#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int
tm_init();

int
start_thread(void * (*start_routine) (void *),
             void *   args);

int
thread_terminated(pthread_t tid);

int
tm_destroy();

