#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <check.h>

#include <util/thread_management.h>
#include <util/config/config_reader.h>
#include <util/logging/logger.h>

void setup(void) {
	int rc;
	rc = cr_init("../../cfg/core_config.xml");
	fail_if(rc == 0, "config reader could not be initalized");
	rc = logger_init();
	fail_if(rc == 0, "logging could not be initialized");
	rc = tm_init();
	fail_if(rc == 0, "thread management could not be initialized");
}

void teardown(void) {
	int rc;
	rc = tm_destroy(0);
	fail_if(rc == 0, "thread management could not be released");
	rc = logger_destroy();
	fail_if(rc == 0, "logging could not be released");
	rc = cr_destroy();
	fail_if(rc == 0, "config reader could not be released");
	pthread_exit(NULL);
}

void *thrd_start_thread(void *args) {
	int rc;
	sleep(2);
	rc = thread_terminated(NULL);
	fail_if(rc == 0, "thread referenced could not be removed");
	return NULL;
}

void *thrd_simple_arguments(void *args) {
	int i;
	int rc;

	i = (int) args;
	fail_unless(i == 2842, "argument has not expected value");
	sleep(2);

	rc = thread_terminated(NULL);
	fail_if(rc == 0, "thread referenced could not be removed");
	return NULL;
}

void *thrd_simple_return_value(void *args) {
	int rc;

	sleep(2);

	rc = thread_terminated((void *) 4711);
	fail_if(rc == 0, "thread referenced could not be removed");
	return NULL;
}

// *INDENT-OFF*

START_TEST(test_start_thread) {
	int rc;
	rc = start_thread(thrd_start_thread, NULL);
	fail_if(rc == 0, "thread could not be started");
} END_TEST 

START_TEST(test_start_multiple_threads) {
	int rc;
	rc = start_thread(thrd_start_thread, NULL);
	fail_if(rc == 0, "thread could not be started");
	rc = start_thread(thrd_start_thread, NULL);
	fail_if(rc == 0, "thread could not be started");
	rc = start_thread(thrd_start_thread, NULL);
	fail_if(rc == 0, "thread could not be started");
	rc = start_thread(thrd_start_thread, NULL);
	fail_if(rc == 0, "thread could not be started");
} END_TEST 

START_TEST(test_simple_arguments) {
	int i;
	int rc;
	i = 2842;
	rc = start_thread(thrd_simple_arguments, (void *) i);
	fail_if(rc == 0, "thread could not be started");
} END_TEST 

START_TEST(test_simple_return_value) {
	int rc;
	rc = start_thread(thrd_simple_return_value, NULL);
	fail_if(rc == 0, "thread could not be started");

} END_TEST 

// *INDENT-ON*

Suite *tm_suite(void) {
	Suite *s = suite_create("thread management\n\n");
	TCase *tc_start = tcase_create("Start Thread");
	TCase *tc_start_multiple = tcase_create("Start Multiple Threads");
	TCase *tc_simple_args = tcase_create("Simple Argument Passing");

	suite_add_tcase(s, tc_start);
	suite_add_tcase(s, tc_start_multiple);
	suite_add_tcase(s, tc_simple_args);

	tcase_add_test(tc_start, test_start_thread);
	tcase_set_timeout(tc_start, 15);
	tcase_add_checked_fixture(tc_start, setup, teardown);

	tcase_add_test(tc_start_multiple, test_start_multiple_threads);
	tcase_set_timeout(tc_start_multiple, 15);
	tcase_add_checked_fixture(tc_start_multiple, setup, teardown);

	tcase_add_test(tc_simple_args, test_simple_arguments);
	tcase_set_timeout(tc_simple_args, 15);
	tcase_add_checked_fixture(tc_simple_args, setup, teardown);

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
