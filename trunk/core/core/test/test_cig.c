#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <../util/thread_management.h>
#include <call_id_generator.h>
#include <time.h>

void test_plain() {
	int id, i;

	for (i = 0; i < 10; i++) {
		id = cig_generate_call_id();
		printf("id: %d\n", id);
	}
}

void *request_id(void *args) {
	int id;

	/*
	 * sleep up to 10 seconds 
	 */
	sleep((int) args);

	id = cig_generate_call_id();
	printf("id: %d\n", id);

	thread_terminated(pthread_self());
	pthread_exit(NULL);
}

void test_threaded() {
	int i, r;

	/*
	 * initialize random generator 
	 */
	srand(time(NULL));

	for (i = 0; i < 10; i++) {
		r = rand() % 20;
		start_thread(request_id, (void *) r);
	}
}

int main() {
	int x;

	tm_init();
	cig_init();

	test_plain();
	test_threaded();

	scanf("%d", &x);

	cig_destroy();
	tm_destroy();
	pthread_exit(NULL);
	return 0;
}
