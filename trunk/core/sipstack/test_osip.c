#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include "sip_stack_interface.h"

// *INDENT-OFF*

START_TEST(test_sipstack_register) {
    /*
     * unit test code
     */

    sipstack_event result;
    int i = sipstack_init();

    /*send initial REGISTER*/
    int regId =
        sipstack_send_register("sip:333@192.168.0.2", "sip:192.168.0.2", 1800);
    fail_unless(regId > -1, "Sending REGISTER failed. (result = %2d)", regId);
    /*receive response*/
    result = sipstack_receive_response(5);
    fail_unless(result.status_code == 200, "No 200 response received. (result = %i)", result.status_code);


    /*update registration*/
    i = sipstack_send_update_register(regId, 1800);
    fail_unless(i == 0, "Updating REGISTER failed. (result = %2d)", i);
    /*receive response*/
    result = sipstack_receive_response(5);
    fail_unless(result.status_code == 200, "No 200 response received. (result = %i)", result.status_code);

    /*unregister*/
    i = sipstack_send_unregister(regId);
    fail_unless(i == 0, "Unregistering failed. (result = %2d)", i);
    /*receive response*/
    result = sipstack_receive_response(5);
    fail_unless(result.status_code == 200, "No 200 response received. (result = %i)", result.status_code);

    sipstack_quit();
}
END_TEST

START_TEST(test_sipstack_call) {
    /*
     * unit test code
     */

    sipstack_event result;
    int i = sipstack_init();

    /*send initial INVITE*/
    int callId =
        sipstack_send_invite("sip:321@192.168.0.2", "sip:123@192.168.0.2", "Sip Stack Test");
    fail_unless(callId > -1, "[test call][INVITE]Sending INVITE failed. (result = %2d)", callId);
    /*receive response*/
    result.status_code = 0;
    while(result.status_code < 200) {
        result = sipstack_receive_response(5);
        /*DEBUG*/
        /*fprintf(stdout, "[test call][INVITE] %i received\n", result.status_code);*/
    }
    fail_unless(result.status_code == 200, "[test call][INVITE]No 200 response for INVITE received. (result = %i)\n", result.status_code);

    /*send ACK for OK*/
    i = sipstack_send_acknowledgment(result.dialogId);
    fail_unless(i == 0, "[test call][INVITE]Sending ACK failed. (result = %2d)", i);
    /*receive response*/
    sipstack_event res = sipstack_receive_response(1);

    /*send BYE*/
    i = sipstack_bye(callId, result.dialogId);
    fail_unless(i == 0, "[test call][BYE]Sending BYE failed. (result = %2d)", i);

    sipstack_quit();
}
END_TEST

// *INDENT-ON*

Suite *
sipstack_suite(void) {
    Suite *s = suite_create("sipstack");
    TCase *tc_apt = tcase_create("Adapter");
    TCase *tc_call = tcase_create("Call");

    suite_add_tcase(s, tc_apt);
    suite_add_tcase(s, tc_call);

    tcase_add_test(tc_apt, test_sipstack_register);

    tcase_set_timeout(tc_call, 30);
    tcase_add_test(tc_call, test_sipstack_call);

    //tcase_add_checked_fixture (tc_apt, setup, teardown);

    return s;
}

int
main(void) {
    int nf;
    Suite *s = sipstack_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (nf == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
