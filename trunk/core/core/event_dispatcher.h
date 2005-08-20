#ifndef HEVENT_DISPATCHER_USED
#define HEVENT_DISPATCHER_USED

#include <events.h>
#include <queue/queue.h>

//int const MAX_CALLS = 32;

typedef struct {
	Queue eventPool;
	pthread_mutex_t poolLock;
	pthread_cond_t wakeUp;
	pthread_mutex_t wakeUpLock;
	int callId;
} sm_data;

int ed_init();

int ed_destroy();

int event_dispatch(event evt, void **params);

#endif
