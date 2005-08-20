#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <../util/thread_management.h>
#include <events.h>
#include <statemachine.h>
#include <call_id_generator.h>
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
create_queue(int *pos, int callId) {
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

    queues[i]->callId = callId;

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

int
find_pos_by_call_id(int callId) {
    int i;

    i = 0;
    while (i < MAX_CALLS) {
        if (queues[i]) {
            if (queues[i]->callId == callId) {
                return i;
            }
        }
        i++;
    }
    return -1;
}

int
enqueue_event(int pos, ElementType * param) {
    pthread_mutex_lock(&queues[pos]->poolLock);
    Enqueue(*param, queues[pos]->eventPool);
    pthread_mutex_unlock(&queues[pos]->poolLock);
    return 1;
}

int
wake_machine(int pos) {
    pthread_mutex_lock(&queues[pos]->wakeUpLock);
    pthread_cond_signal(&queues[pos]->wakeUp);
    pthread_mutex_unlock(&queues[pos]->wakeUpLock);
    return 1;
}

int
enqueue_and_wake(int callId, ElementType * param) {
    int res, pos;
    
    res = find_pos_by_call_id(callId);
    if (res == -1) {
        // ERROR
        return 0;
    }
    pos = res;

    res = enqueue_event(pos, param);
    if (!res) {
        // ERROR
        return 0;
    }

    res = wake_machine(pos);
    if (!res) {
        // ERROR
        return 0;
    }
    return 1;
}

void *
dispatch(void *args) {

    ElementType *param;

    param = (ElementType *) args;

    int res,
      pos,
      callId;

    switch (param->trigger) {
        case GUI_MAKE_CALL:
            callId = (int) param->params[2];
            res = create_queue(&pos, callId);
            if (res == 0) {
                // ERROR
                printf("no free position found!\n");

                free(param);

                thread_terminated(pthread_self());
                pthread_exit(NULL);
            }

            Enqueue(*param, queues[pos]->eventPool);

            start_thread(sm_start, (void *) pos);

            break;
        case SIPLISTENER_INCOMING_CALL:
            callId = (int) param->params[0];
            res = create_queue(&pos, callId);
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
            /*
             * sleep(4);
             * 
             * res = destroy_queue(pos);
             * if (res == 0) {
             * // ERROR
             * printf("error destroying queue!\n");
             * 
             * free(param);
             * 
             * thread_terminated(pthread_self());
             * pthread_exit(NULL);
             * } 
             */
            // </TEST>
            break;
        case GUI_END_CALL:
            callId = (int) param->params[0];
            res = enqueue_and_wake(callId, param);
            if (!res) {
                // ERROR
            }
            break;
        case GUI_ACCEPT_CALL:
            callId = (int) param->params[0];
            res = enqueue_and_wake(callId, param);
            if (!res) {
                // ERROR
            }
            break;
        case SIPLISTENER_CANCEL_CALL:
            callId = (int) param->params[0];
            res = enqueue_and_wake(callId, param);
            if (!res) {
                // ERROR
            }
            break;
        case SIPLISTENER_QUIT_CALL:
            callId = (int) param->params[0];
            res = enqueue_and_wake(callId, param);
            if (!res) {
                // ERROR
            }
            break;
        case SIPLISTENER_DECLINE_CALL:
            callId = (int) param->params[0];
            res = enqueue_and_wake(callId, param);
            if (!res) {
                // ERROR
            }
            break;
        case SIPLISTENER_THROW:
            callId = (int) param->params[0];
            res = enqueue_and_wake(callId, param);
            if (!res) {
                // ERROR
            }
            break;
        case SIPLISTENER_CONNECT_CALL:
            callId = (int) param->params[0];
            res = enqueue_and_wake(callId, param);
            if (!res) {
                // ERROR
            }
            break;
        case INVALID_EVENT:
            // ERROR
            break;
        default:
            // NYI
            break;
    }

    // free(param); ?

    thread_terminated(pthread_self());
    pthread_exit(NULL);
}

int
event_dispatch(event evt, void **params) {

    ElementType *threadParam;
    int result;

    threadParam = (ElementType *) malloc(sizeof(ElementType));
    result = -1;

    if (evt == GUI_MAKE_CALL) {
        // no callId is known - has to be generated, returned to gui and
        // passed to the new statemachine
        int callId,
          accountId;
        char *callee;

        callId = cig_generate_call_id();
        accountId = (int) params[0];
        callee = (char *) params[1];

        threadParam->params =
            (void **) malloc(2 * sizeof(int) + strlen(callee));
        threadParam->params[0] = (void *) accountId;
        threadParam->params[1] = (void *) callee;
        threadParam->params[2] = (void *) callId;
        threadParam->trigger = evt;

        result = callId;
    } else {
        threadParam->trigger = evt;
        threadParam->params = params;
    }


    start_thread(dispatch, (void *) threadParam);

    return result;
}
