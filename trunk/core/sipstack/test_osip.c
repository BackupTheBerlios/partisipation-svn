#include "check.h"
#include "sip_stack_interface.h"

START_TEST (test_osip_ifapt)
{
    /* unit test code */
    register();

    /* start call and quit it*/
    callID = start_call();
    fail_unless(quit_call(callId));

    /* start call and cancel it*/
    callID = start_call();
    fail_unless(cancel_call(callId));

    /* start new SipListener thread and use its id as parameter listener */
    fail_unless(set_listener(listener));

    /* */
    fail_unless();

}
END_TEST
