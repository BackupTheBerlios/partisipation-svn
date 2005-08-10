#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

#include <eXosip2/eXosip.h>
#include "sip_stack_interface.h"

/**
    Init sip stack. This method is supposed to be called before using the sip stack.
*/
int
sipstack_init() {
    int i;

    TRACE_INITIALIZE(6, stdout);

    i = eXosip_init();
    if (i != 0) {
        fprintf(stderr, "\ncould not initialize eXosip\n");
        return -1;
    }
    //i = eXosip_listen_to(IPPROTO_UDP, INADDR_ANY, 5060);
    i = eXosip_listen_addr(IPPROTO_UDP, INADDR_ANY, 5065, AF_INET, 0);
    if (i != 0) {
        eXosip_quit();
        fprintf(stderr, "could not initialize transport layer\n");
        return -1;
    }
}

/**
    Shut down sip stack. This method is supposed to be called if the sip stack is no more needed.
    This includes the case that an error occures.
*/
int
sipstack_quit() {
    /*
     * shut down eXosip
     */
    eXosip_quit();
}

/**
    Send initial register to start a registration.
    After sending the REGISTER this method waits for a response.
    If no response is received in 5 seconds or an error is received this methode returns -1.

    @param char* identity identity of user (e.g. abc@domain.org)
    @param char* registrar server where to register
    @param int expire time to live of registration in seconds
    @return int registration id which can be used to update the registration or to unregister
*/
int
sipstack_send_register(char *const identity, char *const registrar, int expire) {
    osip_message_t *reg = NULL;

    /*
     * registration id
     */
    int id;

    /*
     * return value
     */
    int i;

     /*DEBUG*/
        /*
         * fprintf(stdout, "\n[initial registration] register user...\n");
         */
        /*
         * lock sip stack to avoid conflicts
         */
        eXosip_lock();
    /*
     * build REGISTER message
     */
    id = eXosip_register_build_initial_register(identity, registrar, NULL,
                                                expire, &reg);

    if (id < 0) {
        /*
         * building of REGISTER failed
         */
        /*
         * unlock sip stack for further use
         */
        eXosip_unlock();
        /*
         * return error code
         */
        return -1;
    }

    /*
     * set some properties of message
     */
    osip_message_set_supported(reg, "100rel");
    osip_message_set_supported(reg, "path");

    /*
     * send REGISTER message
     */
    i = eXosip_register_send_register(id, reg);
    /*
     * unlock sip stack for further use
     */
    eXosip_unlock();

    if (i < 0) {
        /*
         * sending of REGISTER message failed
         */
         /*DEBUG*/
            /*
             * fprintf(stdout, "[initial registration] sending REGISTER failed");
             */
            /*
             * return error code
             */
            return -1;
    }

    /*
     * response
     */
    eXosip_event_t *je;

    /*
     * wait for response for 5 seconds
     */
    je = eXosip_event_wait(5, 0);
    /*
     * initiate some automatic actions (e.g. react on 3xx, 401/407)
     */
    eXosip_automatic_action();

    if (je == NULL) {
        /*
         * no response received in time
         */
         /*DEBUG*/
            /*
             * fprintf(stdout, "[initial registration] no response received");
             */
            /*
             * return error code
             */
            return -1;
    } else {
        /*
         * received response
         */
         /*DEBUG*/
            /*
             * fprintf(stdout, "[initial registration] Event '%s' received..\n", je->textinfo);
             */
            /*
             * fprintf(stdout, "[initial registration] response = '%i'\n", je->response->status_code);
             */
    }

     /*DEBUG*/
        /*
         * fprintf(stdout, "[initial registration] ...done\n");
         */
        /*
         * return registration id
         */
        return id;
}

/**
    Deletes a registration. Therefore the expiration time is set to 0.
    After sending the REGISTER this method waits for a response.
    If no response is received in 5 seconds or an error is received this methode returns -1.

    @param int id registration id returned by sipstack_send_register()
    @return int result code
*/
int
sipstack_unregister(int id) {
    osip_message_t *reg = NULL;
    int i;

     /*DEBUG*/
        /*
         * fprintf(stdout, "\n[unregistration] unregister user...\n");
         */
        /*
         * lock sip stack to avoid conflicts
         */
        eXosip_lock();
    /*
     * build REGISTER message
     */
    i = eXosip_register_build_register(id, 0, &reg);

    if (i < 0) {
        /*
         * building of REGISTER failed
         */
        /*
         * unlock sip stack for further use
         */
        eXosip_unlock();
         /*DEBUG*/
            /*
             * fprintf(stderr, "[unregistration] cannot build REGISTER\n");
             */
            /*
             * return error code
             */
            return -1;
    }
    /*DEBUG*/
        /*
         * fprintf(stdout, "[unregistration] REGISTER build\n");
         */
        /*
         * send REGISTER message
         */
        i = eXosip_register_send_register(id, reg);
    /*
     * unlock sip stack for further use
     */
    eXosip_unlock();
    if (i < 0) {
        /*
         * sending of REGISTER message failed
         */
         /*DEBUG*/
            /*
             * fprintf(stdout, "[unregistration] sending REGISTER failed");
             */
            /*
             * return error code
             */
            return -1;
    }

    /*
     * response
     */
    eXosip_event_t *je;

    /*
     * wait for response for 5 seconds
     */
    je = eXosip_event_wait(5, 0);
    /*
     * initiate some automatic actions (e.g. react on 3xx, 401/407)
     */
    eXosip_automatic_action();

    if (je == NULL) {
        /*
         * no response received in time
         */
         /*DEBUG*/
            /*
             * fprintf(stdout, "[unregistration] no response received");
             */
            /*
             * return error code
             */
            return -1;
    } else {
        /*
         * received response
         */
         /*DEBUG*/
            /*
             * fprintf(stdout, "[unregistration] Event '%s' received..\n", je->textinfo);
             */
            /*
             * fprintf(stdout, "[unregistration] response = '%i'\n", je->response->status_code);
             */
    }

     /*DEBUG*/
        /*
         * fprintf(stdout, "[unregistration] ...done\n");
         */
        /*
         * return success code
         */
        return 0;
}

/**
    Updates a registration and sets its expiration time to a given value.
    After sending the REGISTER this method waits for a response.
    If no response is received in 5 seconds or an error is received this methode returns -1.

    @param int id registration id returned by sipstack_send_register()
    @param int expire new expiration time
    @return int result code
*/
int
sipstack_update_register(int id, int expire) {
    osip_message_t *reg = NULL;
    int i;

     /*DEBUG*/
        /*
         * fprintf(stdout, "\n[update registration] update registration of user...\n");
         */
        /*
         * lock sip stack to avoid conflicts
         */
        eXosip_lock();
    /*
     * build REGISTER message
     */
    i = eXosip_register_build_register(id, expire, &reg);

    if (i < 0) {
        /*
         * building of REGISTER failed
         */
        /*
         * unlock sip stack for further use
         */
        eXosip_unlock();
         /*DEBUG*/
            /*
             * fprintf(stderr, "[update registration] cannot build REGISTER\n");
             */
            /*
             * return error code
             */
            return -1;
    }
    /*DEBUG*/
        /*
         * fprintf(stdout, "[update registration] REGISTER build\n");
         */
        /*
         * send REGISTER message
         */
        i = eXosip_register_send_register(id, reg);
    /*
     * unlock sip stack for further use
     */
    eXosip_unlock();
    if (i < 0) {
        /*
         * sending of REGISTER message failed
         */
         /*DEBUG*/
            /*
             * fprintf(stdout, "[update registration] sending REGISTER failed");
             */
            /*
             * return error code
             */
            return -1;
    }

    /*
     * response
     */
    eXosip_event_t *je;

    /*
     * wait for response for 5 seconds
     */
    je = eXosip_event_wait(5, 0);
    /*
     * initiate some automatic actions (e.g. react on 3xx, 401/407)
     */
    eXosip_automatic_action();

    if (je == NULL) {
        /*
         * no response received in time
         */
         /*DEBUG*/
            /*
             * fprintf(stdout, "[update registration] no response received");
             */
            /*
             * return error code
             */
            return -1;
    } else {
        /*
         * received response
         */
         /*DEBUG*/
            /*
             * fprintf(stdout, "[update registration] Event '%s' received..\n", je->textinfo);
             */
            /*
             * fprintf(stdout, "[update registration] response = '%i'\n", je->response->status_code);
             */
    }

     /*DEBUG*/
        /*
         * fprintf(stdout, "[update registration] ...done\n");
         */
        /*
         * return success code
         */
        return 0;
}

/**
    start a new call
*/
int
sipstack_start_call(int id) {
    int callID = id;

    return callID;
}

/**
    cancel a call
*/
int
sipstack_cancel_call(int callId) {
    return 0;
}

/**
    quit a call
*/
int
sipstack_quit_call(int callId) {
    return 0;
}

/**
    decline an incoming call
*/
int
sipstack_decline_call(int callId) {
    return 0;
}

/**
    set sip stack listener (deprecated)
*/
int
sipstack_set_listener(int listener) {
    return 0;
}
