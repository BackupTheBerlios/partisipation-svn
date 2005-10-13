#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <util/threads/thread_management.h>
#include <core/events/event_dispatcher.h>
#include <core/events/events.h>

int main() {
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
	tm_destroy(0);
	free(params1);
	free(params2);
	pthread_exit(NULL);

}
