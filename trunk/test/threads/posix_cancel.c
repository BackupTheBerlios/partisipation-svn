#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>

/* defines the number of searching threads */
#define NUM_THREADS 25

/* function prototypes */
void *search(void *);
void print_it(void *);

/* global variables */
pthread_t  threads[NUM_THREADS];
pthread_mutex_t lock;
int tries;

int
main()
{
int i;
int pid;

/* create a number to search for */
pid = getpid();

/* initialize the mutex lock */
pthread_mutex_init(&lock, NULL); 
printf("Searching for the number = %d...\n", pid);

/* create the searching threads */
for (i=0;i<NUM_THREADS;i++)
	pthread_create(&threads[i], NULL, search, (void *)pid);

/* wait for (join) all the searching threads */
for (i=0;i<NUM_THREADS;i++) 
	pthread_join(threads[i], NULL);

printf("It took %d tries to find the number.\n", tries);

/* exit this thread */
pthread_exit((void *)0);
}

/*
	This is the cleanup function that is called when 
	the threads are cancelled 
*/

void print_it(void *arg)
{
int *try = (int *) arg;
pthread_t tid;

/* get the calling thread's ID */
tid = pthread_self();

/* print where the thread was in its search when it was cancelled */
printf("Thread %d was canceled on its %d try.\n", (int)tid, *try); 
}

/*
	This is the search routine that is executed in each thread 
*/

void *search(void *arg)
{
int num = (int) arg;
int i=0, j;
pthread_t tid;

/* get the calling thread ID */
tid = pthread_self();

/* use the thread ID to set the seed for the random number generator */
srand(tid);

/* set the cancellation parameters --
   - Enable thread cancellation 
   - Defer the action of the cancellation 
*/

pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

/* push the cleanup routine (print_it) onto the thread
   cleanup stack.  This routine will be called when the 
   thread is cancelled.  Also note that the pthread_cleanup_push
   call must have a matching pthread_cleanup_pop call.  The
   push and pop calls MUST be at the same lexical level 
   within the code */

/* pass address of `i' since the current value of `i' is not 
   the one we want to use in the cleanup function */

pthread_cleanup_push(print_it, (void *)&i);

/* loop forever */
while (1) {
	i++;

	/* does the random number match the target number? */
	if (num == rand()) {

		/* try to lock the mutex lock --
                   if locked, check to see if the thread has been cancelled
		   if not locked then continue */

		while (pthread_mutex_trylock(&lock) == EBUSY)
				pthread_testcancel();

		/* set the global variable for the number of tries */

	   	tries = i;

	   	printf("thread %d found the number!\n", (int)tid);

		/* cancel all the other threads */
	   	for (j=0;j<NUM_THREADS;j++) 
			if (threads[j] != tid) pthread_cancel(threads[j]);

		/* break out of the while loop */
		break;
	  	}

	/* every 100 tries check to see if the thread has been cancelled 
           if the thread has not been cancelled then yield the thread's
	   LWP to another thread that may be able to run */

	if (i%100 == 0) {
		pthread_testcancel();
		sched_yield();
		}
	}

/* The only way we can get here is when the thread breaks out
   of the while loop.  In this case the thread that makes it here
   has found the number we are looking for and does not need to run
   the thread cleanup function.  This is why the pthread_cleanup_pop
   function is called with a 0 argument; this will pop the cleanup
   function off the stack without executing it */

pthread_cleanup_pop(0);
return((void *)0);
}
