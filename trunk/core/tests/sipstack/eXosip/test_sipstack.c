#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include <unistd.h>

#include <sipstack/sip_stack_interface.h>

#include <util/config/xml/config_reader.h>
#include <util/threads/thread_management.h>
#include <util/logging/logger.h>

void setup(void) {
	int rc;
	rc = cr_init("../../../config/xml/core_config.xml");
	fail_if(rc == 0, "config reader could not be initialized");
	
	rc = logger_init();
	fail_if(rc == 0, "logger could not be initialized");
	
	rc = tm_init();
	fail_if(rc == 0, "thread management could not be initialized");
}

void teardown(void) {
	int rc;
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

	sipstack_event result;
	int i = sipstack_init(5065);

	/*send initial REGISTER */
	int regId =
		sipstack_send_register("sip:333@192.168.0.2", "sip:192.168.0.2",
							   1800);
	fail_unless(regId > -1, "Sending REGISTER failed. (result = %2d)",
				regId);
	/*receive response */
	result = sipstack_receive_event(5);
	fail_unless(result.statusCode == 200,
				"No 200 response received. (result = %i)",
				result.statusCode);

	/*update registration */
	i = sipstack_send_update_register(regId, 1800);
	fail_unless(i == 0, "Updating REGISTER failed. (result = %2d)", i);
	/*receive response */
	result = sipstack_receive_event(5);
	fail_unless(result.statusCode == 200,
				"No 200 response received. (result = %i)",
				result.statusCode);

	/*unregister */
	i = sipstack_send_unregister(regId);
	fail_unless(i == 0, "Unregistering failed. (result = %2d)", i);
	/*receive response */
	result = sipstack_receive_event(5);
	fail_unless(result.statusCode == 200,
				"No 200 response received. (result = %i)",
				result.statusCode);

	sipstack_quit();
} END_TEST

START_TEST(test_sipstack_call) {
	/*
	 * unit test code
	 */

	sipstack_event result;

	int callId = 2;

	int i = sipstack_init(5065);

	/*send initial INVITE */
	callId = sipstack_send_invite("sip:321@192.168.0.2", "sip:123@192.168.0.2", "Sip Stack Test");
	LOG_DEBUG("%i", callId);
	/*receive response */
	result.statusCode = 0;
	while (result.statusCode < 200) {
		result = sipstack_receive_event(5);
	}
	fail_unless(result.statusCode == 200,
				"[test call][INVITE]No 200 response for INVITE received. (result = %i)\n",
				result.statusCode);

	/*send ACK for OK */
	i = sipstack_send_acknowledgment(result.dialogId);
	fail_unless(i == 0,
				"[test call][INVITE]Sending ACK failed. (result = %2d)", i);
	/*receive response */
	sipstack_event res = sipstack_receive_event(1);

	sleep(2);

	/*send BYE */
	i = sipstack_bye(callId, result.dialogId);
	fail_unless(i == 0,
				"[test call][BYE]Sending BYE failed. (result = %2d)", i);

	sipstack_quit();
} END_TEST

START_TEST(test_sipstack_cancel) {
	/*
	 * unit test code
	 */

	sipstack_event result;

	int i = sipstack_init(5065);

	/*send initial INVITE */
	int callId = sipstack_send_invite("sip:321@192.168.0.2", "sip:123@192.168.0.2", "Sip Stack Test");

	fail_unless(callId > 0, "[test transaction]Sending INVITE failed. (result = %2d)", i);

	/*
		wait for provisional answer (e.g. 100)
		because eXosip can only cancel an inituial INVITE after receiving a provisional answer
	*/
	result.statusCode = 0;
	while (result.statusCode < 99) {
		result = sipstack_receive_event(5);
	}

	/*send CANCEL */
	i = sipstack_cancel(callId, result.dialogId);
	fail_unless(i == 0, "[test transaction]Sending CANCEL failed. (result = %2d)", i);

	/*receive response for INVITE*/
	result.statusCode = 0;
	while (result.statusCode < 487) {
		result = sipstack_receive_event(5);
	}
	fail_unless(result.statusCode == 487,
				"[test call][CANCEL]No 487 response for INVITE received. (result = %i)\n",
				result.statusCode);

} END_TEST


// *INDENT-ON*

Suite *sipstack_suite(void) {
	Suite *s = suite_create("Sipstack (register, call, cancel call)\n");
	TCase *tc_register = tcase_create("Register");
	TCase *tc_call = tcase_create("Call");
	TCase *tc_cancel = tcase_create("Cancel");

	suite_add_tcase(s, tc_register);
	suite_add_tcase(s, tc_call);
	suite_add_tcase(s, tc_cancel);

	tcase_add_test(tc_register, test_sipstack_register);
	tcase_add_checked_fixture(tc_register, setup, teardown);

	tcase_set_timeout(tc_call, 30);
	tcase_add_test(tc_call, test_sipstack_call);
	tcase_add_checked_fixture(tc_call, setup, teardown);

	tcase_set_timeout(tc_cancel, 30);
	tcase_add_test(tc_cancel, test_sipstack_cancel);
	tcase_add_checked_fixture(tc_cancel, setup, teardown);

	//tcase_add_checked_fixture (tc_apt, setup, teardown);

	return s;
}

int main(void) {
	int nf;
	Suite *s = sipstack_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	nf = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
