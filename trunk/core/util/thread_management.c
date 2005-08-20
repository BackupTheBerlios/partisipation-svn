
/** 
 * @file thread_management.c
 * A thread launcher that keeps track of running threads.
 *   
 * This utility file tries to ease the use of threads. Thread references are
 * save for future use like cancelling all threads. Additionally, it abstracts
 * from the used thread library as much as possible.
 *
 * @author Matthias Liebig
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <storage.h>

/**
 * Amount of threads that can run simultaneously.
 */
#define MAXTHREADS 64

/**
 * global lock for thread references
 */
pthread_mutex_t thrManLock;

/**
 * thread references
 */
pthread_t *threads;

/**
 * number of active threads (protected by lock)
 */
int activeThreads;

/**
 * highest number of active threads (protected by lock)
 */
int peak;

/**
 * Private function, do not call. 
 * It handles the work of start_thread() as a separate thread.
 * @param args an instance of thread_data containing the start routine and the
 * arguments of the thread that has to be started
 * @return nothing so far
 */
void *add_thread(void *args) {
	int i, rc;
	thread_data *td;

	td = (thread_data *) args;

	// enter lock to prevent concurring access to thread references
	pthread_mutex_lock(&thrManLock);

	i = 0;
	// find free position in array:
	while (threads[i] != 0) {
		i++;

		if (i == MAXTHREADS) {
			// ERROR
			printf("no free position found!\n");

			// unlock:
			pthread_mutex_unlock(&thrManLock);

			free(td);

			// we're done
			pthread_exit(NULL);
		}
	}

	printf("found free pos: %d\n", i);

	// create desired thread:
	rc = pthread_create(&threads[i], NULL, td->start_routine, td->args);
	if (rc != 0) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
	}

	printf("created thread\n");

	activeThreads++;
	if (activeThreads > peak) {
		peak = activeThreads;
	}
	// unlock:
	pthread_mutex_unlock(&thrManLock);

	free(td);

	// we're done
	pthread_exit(NULL);
}

/**
 * Private function, do not call. 
 * It handles the work of thread_terminated() as a separate thread.
 * @param args the thread id of the thread that will be removed
 * @return nothing so far
 */
void *remove_thread(void *args) {

	pthread_t *tid;

	tid = (pthread_t *) args;

	// enter lock to prevent concurring access to thread references
	pthread_mutex_lock(&thrManLock);

	int i = 0;

	// find position of given thread in array:
	while (threads[i] != *tid) {
		i++;
		if (i == MAXTHREADS) {
			printf("thread not found.\n");
			// unlock:
			pthread_mutex_unlock(&thrManLock);

			free(tid);

			// we're done
			pthread_exit(NULL);
		}
	}

	printf("found thread nr. %d at position %d\n", (int) *tid, i);

	// remove reference:
	threads[i] = 0;

	activeThreads--;

	printf("thread removed\n");

	// unlock:
	pthread_mutex_unlock(&thrManLock);

	free(tid);

	// we're done
	pthread_exit(NULL);
}

/**
 * This function should be called whenever a thread needs to be created.
 * It prevents "fire&forget" thread starting by holding track of currently
 * running threads. It is possible that your thread is not starting 
 * instantly because thread launching is asynchronous.
 * @param start_routine the initial function that will be called when the
 * thread is started
 * @param args the arguments of the start routine - a type-free pointer
 * @return whether starting the thread was successfull (boolean)
 */
int start_thread(void *(*start_routine) (void *), void *args) {
	int rc;
	pthread_t t;
	thread_data *td;

	td = (thread_data *) malloc(sizeof(thread_data));
	td->start_routine = start_routine;
	td->args = args;
	rc = pthread_create(&t, NULL, add_thread, (void *) td);
	if (rc != 0) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		return 0;
	}
	return 1;
}

/**
 * This function should be called by a thread that was launched using the
 * start_thread() function. It signals its termination to the thread 
 * management.
 * @param tid the (own) thread id - usually obtained by calling pthread_self()
 * @return whether removing thread reference was successful (boolean)
 */
int thread_terminated(pthread_t tid) {
	int rc;
	pthread_t t;
	pthread_t *param;

	param = (pthread_t *) malloc(sizeof(pthread_t));
	*param = tid;
	rc = pthread_create(&t, NULL, remove_thread, (void *) param);
	if (rc != 0) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		return 0;
	}
	return 1;
}

/**
 * Initialize thread management. Create lock and reserve memory. Call this
 * before calling any other function of thread management.
 * @return if initialization was successfull (boolean)
 */
int tm_init() {

	// create lock:
	int rc;

	rc = pthread_mutex_init(&thrManLock, NULL);
	if (rc != 0) {
		// ERROR
		return 0;
	}

	activeThreads = 0;
	peak = 0;

	// reserve memory for thread references:
	threads = (pthread_t *) calloc(MAXTHREADS, sizeof(pthread_t));
	return 1;
}

/**
 * Destroy lock and free memory. Call this function when you are done using
 * thread management.
 * @return if releasing thread management was successfull (boolean)
 */
int tm_destroy() {

	// release lock:
	int rc;

	rc = pthread_mutex_destroy(&thrManLock);
	if (rc != 0) {
		// ERROR
		return 0;
	}

	printf("activeThreads: %d\n", activeThreads);
	printf("peak was: %d\n", peak);

	// free memory for thread references
	free(threads);
	return 1;
}
