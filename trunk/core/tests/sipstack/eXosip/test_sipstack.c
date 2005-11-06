#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include <unistd.h>

#include <sipstack/sip_stack_interface.h>
#include <sipstack/sip_listener_interface.h>

#include <util/config/xml/config_reader.h>
#include <util/logging/logger.h>
#include <util/queue/queue.h>
#include <util/threads/thread_management.h>

#define TEST_SIPSTACK_PREFIX "[sipstack test] "
#define TEST_SIPSTACK_HOST "192.168.0.2"
#define TEST_SIPSTACK_USER "333"
#define TEST_SIPSTACK_CALLEE "123"

queue event_queue;

void sip_listener_receive_event(sipstack_event *event) {
	queue_enqueue((void *)event, event_queue);
}

void setup(void) {
	int rc;
	rc = cr_init("../../../config/xml/core_config.xml");
	fail_if(rc == 0, "config reader could not be initialized");
	
	rc = logger_init();
	fail_if(rc == 0, "logger could not be initialized");
	
	//init thread management
	rc = tm_init();
	if (rc == 0) {
		// ERROR
		LOG_ERROR(TEST_SIPSTACK_PREFIX "Thread management could not be initialized.\n");
	}else {
		LOG_DEBUG(TEST_SIPSTACK_PREFIX "Thread management  initialized.\n");
	}

	//init event queue
	event_queue = queue_create_queue(30);
	LOG_DEBUG(TEST_SIPSTACK_PREFIX "Event queue created.");
}

void teardown(void) {
	int rc;

	//empty event queue
	queue_dispose_queue(event_queue);
	LOG_DEBUG(TEST_SIPSTACK_PREFIX "Event queue emptied successfully.");

	rc = tm_destroy(0);
	fail_if(rc == 0, "thread management could not be released");

	rc = logger_destroy();
	fail_if(rc == 0, "logger could not be released");
	
	rc = cr_destroy();
	fail_if(rc == 0, "config reader could not be released");

}

// *INDENT-OFF*

START_TEST(test_sipstack_register) {
	/*
	 * unit test code
	 */

	sipstack_event *event;

	int i = sipstack_init();
	LOG_DEBUG(TEST_SIPSTACK_PREFIX "Sip stack initialized.");

	int counter = 0;

	/*send initial REGISTER */
	int regId =
		sipstack_send_register("sip:"TEST_SIPSTACK_USER"@"TEST_SIPSTACK_HOST, "sip:"TEST_SIPSTACK_HOST, 1800);
	fail_unless(regId > -1, "Sending REGISTER failed. (result = %i)", regId);

	/*receive response */
	while (queue_is_empty(event_queue) && counter < 10) {
		sleep(1);
		counter++;
	}

	fail_unless(!queue_is_empty(event_queue), TEST_SIPSTACK_PREFIX "No response for registering received.");

	event = queue_front_and_dequeue(event_queue);

	fail_unless(event->statusCode == 200,
				"No 200 response for registering received. (result = %i)", event->statusCode);

	/*update registration */
	i = sipstack_send_update_register(regId, 1800);
	fail_unless(i == 1, "Updating registration failed.");

	/*receive response */
	counter = 0;
	while (queue_is_empty(event_queue) && counter < 10) {
		sleep(1);
		counter++;
	}
	fail_unless(!queue_is_empty(event_queue), TEST_SIPSTACK_PREFIX "No response for updating registration received.");

	event = queue_front_and_dequeue(event_queue);

	fail_unless(event->statusCode == 200, TEST_SIPSTACK_PREFIX
				"No 200 response for updating registration received. (result = %i)",
				event->statusCode);

	/*unregister */
	i = sipstack_send_unregister(regId);
	fail_unless(i == 1, "Unregistering failed.");

	/*receive response */
	counter = 0;
	while (queue_is_empty(event_queue) && counter < 10) {
		sleep(1);
		counter++;
	}
	fail_unless(!queue_is_empty(event_queue), TEST_SIPSTACK_PREFIX "No response for unregistering received.");

	event = queue_front_and_dequeue(event_queue);

	fail_unless(event->statusCode == 200, TEST_SIPSTACK_PREFIX
				"No 200 response for unregistering received. (result = %i)",
				event->statusCode);

	sipstack_event_free(event);
	sipstack_quit();
} END_TEST

START_TEST(test_sipstack_call) {
	/*
	 * unit test code
	 */

	sipstack_event * event;

	int statusCode;

	int callId = 2;

	int i = sipstack_init();

	/*send initial INVITE */
	callId = sipstack_send_invite("sip:"TEST_SIPSTACK_CALLEE"@"TEST_SIPSTACK_HOST, "sip:"TEST_SIPSTACK_USER"@"TEST_SIPSTACK_HOST, "Sip Stack Test");
	LOG_DEBUG("INVITE send (callId=%i)", callId);
	/*receive response */
	statusCode = 0;
	while (statusCode < 200) {
		if (!queue_is_empty(event_queue)) {
			event = queue_front_and_dequeue(event_queue);
			statusCode = event->statusCode;
		}
		sleep(1);
	}
	fail_unless(event->statusCode == 200,
				"[test call][INVITE]No 200 response for INVITE received. (result = %i)",
				event->statusCode);

	/*send ACK for OK */
	i = sipstack_send_acknowledgment(event->dialogId);
	fail_unless(i == 1, "[test call][INVITE]Sending ACK failed.");

	sleep(2);

	/*send BYE */
	i = sipstack_bye(callId, event->dialogId);
	fail_unless(i == 1,
				"[test call][BYE]Sending BYE failed. (result = %2d)", i);

	sipstack_event_free(event);
	sipstack_quit();
} END_TEST

START_TEST(test_sipstack_cancel) {
	/*
	 * unit test code
	 */

	sipstack_event * event;

	int statusCode;

	int i = sipstack_init();

	/*send initial INVITE */
	int callId = sipstack_send_invite("sip:"TEST_SIPSTACK_CALLEE"@"TEST_SIPSTACK_HOST, "sip:"TEST_SIPSTACK_USER"@"TEST_SIPSTACK_HOST, "Sip Stack Test");

	fail_unless(callId > 0, "[test transaction]Sending INVITE failed. (result = %2d)", i);

	/*
		wait for provisional answer (e.g. 100)
		because eXosip can only cancel an inituial INVITE after receiving a provisional answer
	*/
	statusCode = 0;
	while (statusCode < 99) {
		if (!queue_is_empty(event_queue)) {
			event = queue_front_and_dequeue(event_queue);
			statusCode = event->statusCode;
		}
		sleep(1);
	}

	/*send CANCEL */
	i = sipstack_cancel(callId, event->dialogId);
	fail_unless(i == 1, "[test transaction]Sending CANCEL failed.");

	/*receive response for INVITE*/
	statusCode = 0;
	while (statusCode < 487) {
		if (!queue_is_empty(event_queue)) {
			event = queue_front_and_dequeue(event_queue);
			statusCode = event->statusCode;
		}
		sleep(1);
	}
	fail_unless(event->statusCode == 487,
				"[test call][CANCEL]No 487 response for INVITE received. (received %i event)",
				event->statusCode);
	/*send no ACK for 487 because eXosip already did*/
	sipstack_event_free(event);
	sipstack_quit();

} END_TEST

START_TEST(test_sipstack_incoming_call) {
	/*
	 * unit test code
	 */
	sipstack_event * event;

	int i = sipstack_init();
	int counter = 0;

	
	/*registering at registrar*/
	
	/*send initial REGISTER */
	int regId =
		sipstack_send_register("sip:"TEST_SIPSTACK_USER"@"TEST_SIPSTACK_HOST, "sip:"TEST_SIPSTACK_HOST, 1800);
	fail_unless(regId > -1, "Sending REGISTER failed. (result = %i)", regId);

	/*receive response */
	while (queue_is_empty(event_queue) && counter < 20) {
		sleep(1);
		counter++;
	}

	fail_unless(!queue_is_empty(event_queue), TEST_SIPSTACK_PREFIX "No response for registering received.");

	event = queue_front_and_dequeue(event_queue);

	fail_unless(event->statusCode == 200,
				"No 200 response for registering received. (result = %i)", event->statusCode);



	printf("Please send INVITE now.\n");

	/*wait for an INVITE*/
	counter = 0;
	while (event->type != EXOSIP_CALL_INVITE && counter < 20) {
		if (!queue_is_empty(event_queue)) {
			event = queue_front_and_dequeue(event_queue);
		}
		sleep(1);
		counter++;
	}

	fail_unless(event->type == EXOSIP_CALL_INVITE, "[test incoming call]No INVITE received.");

	/*send OK for INVITE */
	i = sipstack_send_ok(event->dialogId, event->transactionId);

	fail_unless(i == 1, "[test incoming call]Sending of OK failed.");

	/*wait for ACK*/
	counter = 0;
	while (event->type != EXOSIP_CALL_ACK && counter < 10) {
		if (!queue_is_empty(event_queue)) {
			event = queue_front_and_dequeue(event_queue);
		}
		sleep(1);
		counter++;
	}

	fail_unless(event->type == EXOSIP_CALL_ACK,
				"[test incoming call]No ACK received.");

	sleep(2);

	/*send BYE */
	i = sipstack_bye(event->callId, event->dialogId);
	fail_unless(i == 1,
				"[test incoming call][BYE]Sending BYE failed");
	
	sipstack_event_free(event);
	sipstack_quit();

} END_TEST


/* ************************************************************ */
/* bug tests                                                    */
/* ************************************************************ */
START_TEST(test_bug0001) {
	/*
	 * BUG 0001: Segfault after sending REGISTER to an invalid address (no registrar aat this address) and waiting >30sec
	 */

	sipstack_event *event;
	event = (sipstack_event *) malloc(sizeof(sipstack_event));

	int i = sipstack_init();

	int counter = 0;

	/*send REGISTER */
	int regId =
		sipstack_send_register("sip:"TEST_SIPSTACK_USER"@192.168.0.1", "sip:192.168.0.99", 1800);
	fail_unless(regId > -1, "Sending REGISTER failed. (result = %i)", regId);

	/*receive response */
	while (queue_is_empty(event_queue) && counter < 60) {
		sleep(1);
		counter++;
	}

	fail_unless(!queue_is_empty(event_queue), TEST_SIPSTACK_PREFIX "No response for registering received.");

	event = queue_front_and_dequeue(event_queue);

	/*expect REGISTRATION_FAILURE (2)*/
	fail_unless(event->type == 2,
				"No event of type REGISTRATION_FAILURE received. (type = %i)", event->type);

	sipstack_event_free(event);
	sipstack_quit();
} END_TEST

// *INDENT-ON*







Suite *sipstack_suite(void) {
	Suite *s = suite_create("Sipstack (register, call, cancel call)\n");

	TCase *tc_register = tcase_create("Register");
	TCase *tc_call = tcase_create("Call");
	TCase *tc_cancel = tcase_create("Cancel");
	TCase *tc_incoming_call = tcase_create("Incoming call");

	suite_add_tcase(s, tc_register);
	suite_add_tcase(s, tc_call);
	suite_add_tcase(s, tc_cancel);
	suite_add_tcase(s, tc_incoming_call);

	tcase_set_timeout(tc_register, 30);
	tcase_add_test(tc_register, test_sipstack_register);
	tcase_add_checked_fixture(tc_register, setup, teardown);

	tcase_set_timeout(tc_call, 30);
	tcase_add_test(tc_call, test_sipstack_call);
	tcase_add_checked_fixture(tc_call, setup, teardown);

	tcase_set_timeout(tc_cancel, 30);
	tcase_add_test(tc_cancel, test_sipstack_cancel);
	tcase_add_checked_fixture(tc_cancel, setup, teardown);

	tcase_set_timeout(tc_incoming_call, 60);
	tcase_add_test(tc_incoming_call, test_sipstack_incoming_call);
	tcase_add_checked_fixture(tc_incoming_call, setup, teardown);

	//tcase_add_checked_fixture (tc_apt, setup, teardown);

	return s;
}


Suite *sipstack_bug_suite(void) {
	Suite *s = suite_create("Check for known bugs\n");

	TCase *tc_bug0001 = tcase_create("Bug0001");

	suite_add_tcase(s, tc_bug0001);

	tcase_set_timeout(tc_bug0001, 120);
	tcase_add_test(tc_bug0001, test_bug0001);
	tcase_add_checked_fixture(tc_bug0001, setup, teardown);

	return s;
}





int main(void) {
	
	int nf1 = 0;
	Suite *s1 = sipstack_suite();
	SRunner *sr1 = srunner_create(s1);
	srunner_run_all(sr1, CK_NORMAL);
	nf1 = srunner_ntests_failed(sr1);
	srunner_free(sr1);

	//check for known bugs
	int nf2 = 0;
	Suite *s2 = sipstack_bug_suite();
	SRunner *sr2 = srunner_create(s2);
	srunner_run_all(sr2, CK_NORMAL);
	nf2 = srunner_ntests_failed(sr2);
	srunner_free(sr2);
	
	return (nf1+nf2 == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
