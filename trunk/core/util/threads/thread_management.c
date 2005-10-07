
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

#include <util/logging/logger.h>
#include <util/threads/thread_management.h>
#include <util/threads/tm_storage.h>

/**
 * Amount of threads that can run simultaneously.
 */
#define MAXTHREADS 64

/**
 * global lock for thread references
 */
pthread_mutex_t thrManLock;

/**
 * lock for preventing new threads from starting while shutting down
 */
pthread_mutex_t startLock;

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
 * whether thread management is shutting down (boolean)
 */
int shuttingDown;

int *haveCleaner;

int find_pos_by_thread_id(pthread_t tid) {
	int i = 0;
	while (threads[i] != tid) {
		i++;
		if (i == MAXTHREADS) {
			return -1;
		}
	}
	return i;
}

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
	pthread_attr_t attr;

	td = (thread_data *) args;

	pthread_mutex_lock(&startLock);

	if (shuttingDown) {
		pthread_mutex_unlock(&startLock);
		free(td);

		// we're done
		pthread_exit(NULL);
		return NULL;
	}

	pthread_mutex_unlock(&startLock);

	// enter lock to prevent concurring access to thread references
	pthread_mutex_lock(&thrManLock);

	i = 0;
	// find free position in array:
	while (threads[i] != 0) {
		i++;

		if (i == MAXTHREADS) {
			// ERROR
			LOG_ERROR(THREAD_MGMT_MSG_PREFIX "no free position found!\n");

			// unlock:
			pthread_mutex_unlock(&thrManLock);

			free(td);

			// we're done
			pthread_exit(NULL);
		}
	}

	LOG_INFO(THREAD_MGMT_MSG_PREFIX "found free pos: %d\n", i);

	// initialize and set thread detached attribute
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	// create desired thread:
	rc = pthread_create(&threads[i], &attr, td->start_routine, td->args);
	if (rc != 0) {
		LOG_ERROR(THREAD_MGMT_MSG_PREFIX
				  "return code from pthread_create() is %d\n", rc);
	}
	// free attribute
	pthread_attr_destroy(&attr);

	LOG_INFO(THREAD_MGMT_MSG_PREFIX "created thread\n");

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
	LOG_DEBUG(THREAD_MGMT_MSG_PREFIX "start_thread() called");

	int rc;
	pthread_t t;
	thread_data *td;

	td = (thread_data *) malloc(sizeof(thread_data));
	td->start_routine = start_routine;
	td->args = args;
	rc = pthread_create(&t, NULL, add_thread, (void *) td);
	if (rc != 0) {
		LOG_ERROR(THREAD_MGMT_MSG_PREFIX
				  "return code from pthread_create() is %d\n", rc);
		return 0;
	}

	LOG_DEBUG(THREAD_MGMT_MSG_PREFIX "leaving start_thread()");
	return 1;
}

/**
 * This function should be called by a thread that was launched using the
 * start_thread() function. It signals its termination to the thread 
 * management.
 * @return whether removing thread reference was successful (boolean)
 */
int thread_terminated() {
	LOG_DEBUG(THREAD_MGMT_MSG_PREFIX "thread_terminated() called");

	int pos;
	pos = find_pos_by_thread_id(pthread_self());
	if (pos == -1) {
		LOG_ERROR(THREAD_MGMT_MSG_PREFIX "thread not found.\n");
		return 0;
	}

	LOG_INFO(THREAD_MGMT_MSG_PREFIX "found thread No. %d at position %d\n",
			 (int) threads[pos], pos);

	// enter lock to prevent concurring access to thread references
	pthread_mutex_lock(&thrManLock);

	// remove reference:
	threads[pos] = 0;

	activeThreads--;

	LOG_INFO(THREAD_MGMT_MSG_PREFIX "thread removed\n");

	// unlock:
	pthread_mutex_unlock(&thrManLock);

	LOG_DEBUG(THREAD_MGMT_MSG_PREFIX "leaving thread_terminated()");
	if (haveCleaner[pos]) {
		//pthread_cleanup_pop(0);
		haveCleaner[pos] = 0;
	}

	pthread_exit(NULL);
	return 1;
}

/**
 * Initialize thread management. Create lock and reserve memory. Call this
 * before calling any other function of thread management.
 * @return if initialization was successfull (boolean)
 */
int tm_init() {

	LOG_DEBUG(THREAD_MGMT_MSG_PREFIX "initialize thread management");

	// create locks:
	int rc;

	rc = pthread_mutex_init(&thrManLock, NULL);
	if (rc != 0) {
		LOG_ERROR(THREAD_MGMT_MSG_PREFIX
				  "failed to initialize mutex lock\n");
		return 0;
	}

	rc = pthread_mutex_init(&startLock, NULL);
	if (rc != 0) {
		LOG_ERROR(THREAD_MGMT_MSG_PREFIX
				  "failed to initialize mutex lock\n");
		return 0;
	}

	activeThreads = 0;
	peak = 0;
	shuttingDown = 0;

	// reserve memory for thread references:
	threads = (pthread_t *) calloc(MAXTHREADS, sizeof(pthread_t));

	haveCleaner = (int *) calloc(MAXTHREADS, sizeof(int));
	return 1;
}

int shutdown_threads() {
	return 1;
}

/**
 * Join all remaining threads that are still running.
 * @return whether joining all threads was successful (boolean)
 */
int join_threads() {
	int i;

	int rc;
	for (i = 0; i < MAXTHREADS; i++) {
		pthread_t tid = threads[i];
		if (tid) {
			rc = pthread_join(tid, NULL);
			if (rc) {
				LOG_ERROR(THREAD_MGMT_MSG_PREFIX
						  "error joining thread No. %d, return code from "
						  "pthread_join() is %d", (int) tid, rc);
				return 0;
			}
			LOG_DEBUG(THREAD_MGMT_MSG_PREFIX "joined thread No. %d",
					  (int) tid);
		}
	}
	return 1;
}

/**
 * This waits on all running threads until they are finished. It does not 
 * prevent new threads threads from starting, so it is possible that there are
 * still threads running after this function. This only happens if you have 
 * threads starting other threads. In this case you can ensure the termination
 * of all threads if you call tm_destroy().<br \>
 * If you start threads right before this routine, it is wise to insert a 
 * sleep() right before it to ensure that all threads are really started.
 * @return if joining all threads was successful (boolean)
 */
int tm_join_active_threads() {
	LOG_DEBUG(THREAD_MGMT_MSG_PREFIX "entering tm_join_active_threads()");
	int threadsRunning;			// whether there are threads running
	int rc;						// return code

	// only lock for getting the number of active threads, because we don't
	// want to block starting or removing managed threads:
	threadsRunning = 0;
	pthread_mutex_lock(&thrManLock);
	threadsRunning = activeThreads > 0;
	pthread_mutex_unlock(&thrManLock);

	if (threadsRunning) {
		// there are working threads:
		LOG_DEBUG(THREAD_MGMT_MSG_PREFIX
				  "waiting for all threads to finish");
		rc = join_threads();
		if (!rc) {
			LOG_ERROR(THREAD_MGMT_MSG_PREFIX
					  "failed to wait for all threads");
			return 0;
		}
	}

	LOG_DEBUG(THREAD_MGMT_MSG_PREFIX "leaving tm_join_active_threads()");
	return 1;
}

/**
 * Destroy lock and free memory. Call this function when you are done using
 * thread management. It is not possible to start new threads after invoking 
 * this.
 * @param forceShutdown whether all running threads shall be cancelled or if
 * all threads will be joined, i.e. we wait until they are done (boolean)
 * @return if releasing thread management was successfull (boolean)
 */
int tm_destroy(int forceShutdown) {
	LOG_DEBUG(THREAD_MGMT_MSG_PREFIX "releasing thread management");

	int rc;						// return code
	int threadsRunning;			// are there still threads running (boolean)

	// prevent that tm_destroy is called too early:
	sched_yield();

	// prevent new threads from starting:
	pthread_mutex_lock(&startLock);
	shuttingDown = 1;
	pthread_mutex_unlock(&startLock);

	// check whether threads are running
	threadsRunning = 0;
	pthread_mutex_lock(&thrManLock);
	threadsRunning = activeThreads > 0;
	pthread_mutex_unlock(&thrManLock);

	// unlock because remove_thread() is allowed

	if (threadsRunning) {
		if (forceShutdown) {
			// don't wait, shut down now
			LOG_DEBUG(THREAD_MGMT_MSG_PREFIX
					  "forcing shutdown of all threads");
			rc = shutdown_threads();
			if (!rc) {
				LOG_ERROR(THREAD_MGMT_MSG_PREFIX
						  "failed to shut down all threads");
			}
		} else {
			// wait for all remaining threads
			LOG_DEBUG(THREAD_MGMT_MSG_PREFIX
					  "waiting for all threads to finish");
			rc = join_threads();
			if (!rc) {
				LOG_ERROR(THREAD_MGMT_MSG_PREFIX
						  "failed to wait for all threads");
			}
		}

		// now wait on all remaining remove_thread() instances 
		pthread_mutex_lock(&thrManLock);

		while (activeThreads > 0) {
			pthread_mutex_unlock(&thrManLock);
			// activate a remove_thread()-thread
			sched_yield();
			pthread_mutex_lock(&thrManLock);
		}
		pthread_mutex_unlock(&thrManLock);
	}
	// now all threads are finished, references cleared

	// release locks:
	rc = pthread_mutex_destroy(&thrManLock);
	if (rc != 0) {
		LOG_ERROR(THREAD_MGMT_MSG_PREFIX "failed to release mutex lock\n");
		return 0;
	}

	rc = pthread_mutex_destroy(&startLock);
	if (rc != 0) {
		LOG_ERROR(THREAD_MGMT_MSG_PREFIX "failed to release mutex lock\n");
		return 0;
	}

	LOG_INFO(THREAD_MGMT_MSG_PREFIX "activeThreads: %d\n", activeThreads);
	LOG_INFO(THREAD_MGMT_MSG_PREFIX "peak was: %d\n", peak);

	// free memory for thread references
	free(threads);
	return 1;
}

int tm_enable_cancelability() {
	int rc;

	rc = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (rc != 0) {
		LOG_ERROR(THREAD_MGMT_MSG_PREFIX
				  "could not set cancelstate to enable of thread No. %d,"
				  " return value was %d", pthread_self(), rc);
		return 0;
	}

	rc = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
	if (rc != 0) {
		LOG_ERROR(THREAD_MGMT_MSG_PREFIX
				  "could not set canceltype to deferred of thread No. %d, "
				  "return value was %d", pthread_self(), rc);
		return 0;
	}

	return 1;
}

int tm_disable_cancelability() {
	int rc;

	rc = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	if (rc != 0) {
		LOG_ERROR(THREAD_MGMT_MSG_PREFIX
				  "could not set cancelstate to enable of thread No. %d,"
				  " return value was %d", pthread_self(), rc);
		return 0;
	}

	return 1;
}

void tm_cancellation_point() {
	pthread_testcancel();
}

int tm_set_cleaner(void (*routine) (void *), void *arg) {
	int pos;

	pos = find_pos_by_thread_id(pthread_self());
	if (pos == -1) {
		LOG_ERROR(THREAD_MGMT_MSG_PREFIX
				  "could not retrieve thread to set cleanup routine");
		return 0;
	}

	haveCleaner[pos] = 1;

	//pthread_cleanup_push(routine, arg);
	return 1;
}
