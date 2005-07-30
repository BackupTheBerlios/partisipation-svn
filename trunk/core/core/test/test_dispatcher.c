#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <../util/thread_management.h>
#include <event_dispatcher.h>
#include <events.h>

int
main() {
    int x;

    tm_init();
    ed_init();
    void **params1;
    params1 = (void **) malloc(sizeof(int));
    params1[0] = (void *) 27;
    void **params2;
    params2 = (void **) malloc(sizeof(int));
    params2[0] = (void *) 99;
    event_dispatch(GUI_MAKE_CALL, params1);
    event_dispatch(GUI_MAKE_CALL, params2);
    scanf("%d", &x);
    ed_destroy();
    tm_destroy();
    free(params1);
    free(params2);
    pthread_exit(NULL);

}
