#ifndef HEVENT_DISPATCHER_USED
#define HEVENT_DISPATCHER_USED

#include <core/events.h>
#include <util/queue/queue.h>

//int const MAX_CALLS = 32;

typedef struct {
	queue eventPool;
	pthread_mutex_t poolLock;
	pthread_cond_t wakeUp;
	pthread_mutex_t wakeUpLock;
	int callId;
} sm_data;

typedef struct {
	event trigger;
	void **params;
} call_trigger;

int ed_init();

int ed_destroy();

int event_dispatch(event evt, void **params);

#endif
