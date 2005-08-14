#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <../util/thread_management.h>
#include <events.h>
#include <statemachine.h>
#include <event_dispatcher.h>

int const MAX_CALLS = 32;
int const MAX_EVENTS = 16;

sm_data **queues;
pthread_mutex_t queuesLock;

int
ed_init() {
    int rc;

    rc = pthread_mutex_init(&queuesLock, NULL);
    if (rc != 0) {              // ERROR
        return 0;
    }

    queues = (sm_data **) calloc(MAX_CALLS, sizeof(sm_data *));

    return 1;
}

int
ed_destroy() {
    int rc;

    rc = pthread_mutex_destroy(&queuesLock);
    if (rc != 0) {
        // ERROR
        return 0;
    }

    free(queues);

    return 1;
}

int
create_queue(int *pos) {
    int i,                      // counter
      rc;                       // return code

    // lock write access to the queues array
    pthread_mutex_lock(&queuesLock);

    i = 0;
    // find free position in array:
    while (queues[i] != 0) {
        i++;

        if (i == MAX_CALLS) {
            // ERROR: too many simultanous calls
            // unlock:
            pthread_mutex_unlock(&queuesLock);
            return 0;
        }
    }

    printf("position found: %d\n", i);

    sm_data *data;

    // reserve memory for statemachine-specific data
    data = (sm_data *) malloc(sizeof(sm_data));
    queues[i] = data;

    // now the position is marked as used and cannot be overwritten:
    // we can unlock
    pthread_mutex_unlock(&queuesLock);

    // initialize event queue for specific statemachine:
    queues[i]->eventPool = CreateQueue(MAX_EVENTS);

    // initialize wakeup-condition-variable for specific statemachine:
    rc = pthread_cond_init(&queues[i]->wakeUp, NULL);
    if (rc != 0) {
        // ERROR
        return 0;
    }
    // condition-variable always comes with a mutex lock:
    rc = pthread_mutex_init(&queues[i]->wakeUpLock, NULL);
    if (rc != 0) {
        // ERROR
        return 0;
    }
    // initialize mutex lock for event pool:
    rc = pthread_mutex_init(&queues[i]->poolLock, NULL);
    if (rc != 0) {
        // ERROR
        return 0;
    }
    // set the given parameter to the found position
    *pos = i;

    // return true for success:
    return 1;
}

int
destroy_queue(int pos) {
    int rc;                     // return code

    // release queue (event pool):
    DisposeQueue(queues[pos]->eventPool);

    // release condition variable:
    rc = pthread_cond_destroy(&queues[pos]->wakeUp);
    if (rc != 0) {
        // ERROR
        return 0;
    }
    // release mutex locks:
    rc = pthread_mutex_destroy(&queues[pos]->wakeUpLock);
    if (rc != 0) {
        // ERROR
        return 0;
    }

    rc = pthread_mutex_destroy(&queues[pos]->poolLock);
    if (rc != 0) {
        // ERROR
        return 0;
    }
    // release array element and remove reference:
    free(queues[pos]);
    queues[pos] = 0;

    // return true for success:
    return 1;
}

void *
dispatch(void *args) {

    ElementType *param;

    param = (ElementType *) args;

    int res,
      pos;

    switch (param->trigger) {
        case GUI_MAKE_CALL:
        case SIPLISTENER_INCOMING_CALL:
            res = create_queue(&pos);
            if (res == 0) {
                // ERROR
                printf("no free position found!\n");

                free(param);

                thread_terminated(pthread_self());
                pthread_exit(NULL);
            }

            Enqueue(*param, queues[pos]->eventPool);

            start_thread(sm_start, (void *) pos);

            // <TEST>
            sleep(4);

            res = destroy_queue(pos);
            if (res == 0) {
                // ERROR
                printf("error destroying queue!\n");

                free(param);

                thread_terminated(pthread_self());
                pthread_exit(NULL);
            }
            // </TEST>
            break;
        case INVALID_EVENT:
            break;
        default:
            break;
    }

    free(param);

    thread_terminated(pthread_self());
    pthread_exit(NULL);
}

void
event_dispatch(event evt, void **params) {

    ElementType *threadParam;

    threadParam = (ElementType *) malloc(sizeof(ElementType));
    threadParam->trigger = evt;
    threadParam->params = params;

    start_thread(dispatch, (void *) threadParam);
}
