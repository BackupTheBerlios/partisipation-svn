#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include "sip_stack_interface.h"

// *INDENT-OFF*

START_TEST(test_sipstack_ifapt) {
    /*
     * unit test code 
     */

    int i = sipstack_init();

    int regId =
        sipstack_send_register("sip:321@192.168.0.2", "sip:192.168.0.2", 1800);
    fail_unless(regId > -1, "Sending REGISTER failed. (result = %2d)", regId);
    i = sipstack_update_register(regId, 1800);
    fail_unless(i == 0, "Updating REGISTER failed. (result = %2d)", i);
    i = sipstack_unregister(regId);
    fail_unless(i == 0, "Unregistering failed. (result = %2d)", i);

    i = sipstack_quit();

    /*
     * start call and quit it
     */
    //int callID = start_call();

    /*
     * start call and cancel it
     */
    //callID = start_call();
    //fail_unless(cancel_call(callID));

    /*
     * start new SipListener thread and use its id as parameter listener 
     */
    //fail_unless(set_listener(listener));

    /*
     */
    //fail_unless();

}
END_TEST 

// *INDENT-ON*

Suite *
sipstack_suite(void) {
    Suite *s = suite_create("sipstack");
    TCase *tc_apt = tcase_create("Adapter");

    suite_add_tcase(s, tc_apt);

    tcase_add_test(tc_apt, test_sipstack_ifapt);

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
