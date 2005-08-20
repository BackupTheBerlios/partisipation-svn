#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#include <../util/thread_management.h>
#include <call_id_generator.h>

int callId;
pthread_mutex_t idLock;

int
cig_init() {
    int rc;                     // return code

    callId = 0;

    rc = pthread_mutex_init(&idLock, NULL);
    if (rc != 0) {
        // ERROR
        return 0;
    }
    return 1;
}

int
cig_generate_call_id() {
    int result;

    pthread_mutex_lock(&idLock);

    if (callId < INT_MAX) {
        callId++;
    } else {
        callId = 1;
    }
    result = callId;

    pthread_mutex_unlock(&idLock);

    return result;
}


int
cig_destroy() {
    int rc;                     // return code

    rc = pthread_mutex_destroy(&idLock);
    if (rc != 0) {
        // ERROR
        return 0;
    }

    return 1;
}
