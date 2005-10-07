#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <check.h>
#include <time.h>

#include <util/thread_management.h>
#include <util/logging/logger.h>
#include <util/config/config_reader.h>
#include <core/call_id_generator.h>

#define MAX_IDS_PLAIN (10)
#define MAX_IDS_THREADED (10)

int testIds[MAX_IDS_THREADED];
int testIdsSize;
pthread_mutex_t idsLock;

void setup(void) {
	int rc;

	rc = cig_init();
	fail_if(rc == 0, "call id generator could not be initialized");
}

void teardown(void) {
	int rc;

	rc = cig_destroy();
	fail_if(rc == 0, "call id generator could not be released");
}

void setup_with_tm(void) {
	int rc;

	rc = cr_init("../../cfg/core_config.xml");
	fail_if(rc == 0, "config reader could not be initalized");

	rc = logger_init();
	fail_if(rc == 0, "logging could not be initialized");

	rc = tm_init();
	fail_if(rc == 0, "thread management could not be initialized");

	rc = pthread_mutex_init(&idsLock, NULL);
	fail_unless(rc == 0, "mutex could not be initialized");

	setup();
}

void teardown_with_tm(void) {
	int rc;

	teardown();

	rc = pthread_mutex_destroy(&idsLock);
	fail_unless(rc == 0, "mutex could not be released");

	rc = tm_destroy(0);
	fail_if(rc == 0, "thread management could not be released");

	rc = logger_destroy();
	fail_if(rc == 0, "logging could not be released");

	rc = cr_destroy();
	fail_if(rc == 0, "config reader could not be released");

	pthread_exit(NULL);
}

void *thrd_request_id(void *args) {
	int id;

	// sleep up to 15 seconds:
	sleep((int) args);

	id = cig_generate_call_id();

	pthread_mutex_lock(&idsLock);
	testIds[testIdsSize] = id;
	testIdsSize++;
	pthread_mutex_unlock(&idsLock);

	thread_terminated();
	return NULL;
}

// *INDENT-OFF*

START_TEST(test_plain) {
	int i, j;
	int ids[MAX_IDS_PLAIN];

	for (i = 0; i < MAX_IDS_PLAIN; i++) {
		ids[i] = cig_generate_call_id();
	}

	for (i = 0; i < MAX_IDS_PLAIN - 1; i++) {
		for (j = i + 1; j < MAX_IDS_PLAIN; j++) {
			fail_if(ids[i] == ids[j],
					"Identical IDs found - should never occur");
		}
	}
} END_TEST 

START_TEST(test_threaded) {
	int i, j, r;

	testIdsSize = 0;

	// initialize random generator: 
	srand(time(NULL));

	for (i = 0; i < MAX_IDS_THREADED; i++) {
		r = rand() % 15;
		start_thread(thrd_request_id, (void *) r);
	}
	
	// ensure creation of threads:
	sleep(2);
	// wait for all threads to be finished:
	tm_join_active_threads();

	for (i = 0; i < MAX_IDS_THREADED - 1; i++) {
		for (j = i + 1; j < MAX_IDS_THREADED; j++) {
			fail_if(testIds[i] == testIds[j],
					"Identical IDs found - should never occur");
		}
	}

} END_TEST 

// *INDENT-ON*

Suite *tm_suite(void) {
	Suite *s = suite_create("call id generation\n\n");
	TCase *tc_plain = tcase_create("Test plain generation");
	TCase *tc_threaded = tcase_create("Test threaded generation");

	suite_add_tcase(s, tc_plain);
	suite_add_tcase(s, tc_threaded);

	tcase_add_test(tc_plain, test_plain);
	tcase_add_checked_fixture(tc_plain, setup, teardown);

	tcase_add_test(tc_threaded, test_threaded);
	tcase_set_timeout(tc_threaded, 30);
	tcase_add_checked_fixture(tc_threaded, setup_with_tm,
							  teardown_with_tm);

	return s;
}

int main(void) {
	int nf;
	Suite *s = tm_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	nf = srunner_ntests_failed(sr);
	srunner_free(sr);
	if (nf != 0) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
