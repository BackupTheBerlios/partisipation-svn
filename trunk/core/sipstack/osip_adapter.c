#include <stdlib.h>
#include <stdio.h>

#include <sip_stack_interface.h>

int send_register(int const accountId,
          char* const callee) {

}

int start_call() {
    int callID = 4;
    return callID;
}

int cancel_call(int callId) {
    return 1;
}

int quit_call(int callId) {
    return 1;
}

int decline_call(int callId) {
    return 1;
}

int set_listener(int listener) {
    return 1;
}


int
make_call(int    const accountId,
          char * const callee) {

    // do something

    printf("accountId: %d\n", accountId);
    printf("callee: %s\n", callee);

    return 4711;
}

int
end_call(int const callId) {

    // do something

    /* Complete GUI Test */
    int test;
    test = change_reg_status(1, 0);
    test = change_call_status(2727, "TRYING");
    test = show_user_event(7, "INFO", "some title", "some message", 
                           "some detail message");
    test = register_core();
    test = incoming_call(5, 9090, "some callee", "john doe");
    test = set_speaker_volume_cb(0.5);
    test = set_micro_volume_cb(0.25);
    /* end of GUI Test */
    
    printf("callId: %d\n", callId);
    
    return 1;
}

int
accept_call(int const callId) {
          
    // do something
    
    printf("callId: %d\n", callId);
        
    return 1;
}
