#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

#include <eXosip2/eXosip.h>
#include "sip_stack_interface.h"

int
sipstack_init() {
    /*
     * init eXosip2 and osip
     */
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

int
sipstack_quit() {
    eXosip_quit();
}
/**
    send initial register
    @param char* identity
    @param char* registrar
    @param int expire
*/
int
sipstack_send_register(char *const identity, char *const registrar, int expire) {
    osip_message_t *reg = NULL;
    int id;
    int i;

    /*DEBUG*/
    /*fprintf(stdout, "\n[initial registration] register user...\n");*/

    eXosip_lock();
    id = eXosip_register_build_initial_register(identity, registrar, NULL,
                                                expire, &reg);

    if (id < 0) {
        eXosip_unlock();
        return -1;
    }

    osip_message_set_supported(reg, "100rel");
    osip_message_set_supported(reg, "path");

    i = eXosip_register_send_register(id, reg);
    eXosip_unlock();

    if (i < 0) {
        eXosip_quit();
        /*DEBUG*/
        /*fprintf(stdout, "[initial registration] sending REGISTER failed");*/
        return -1;
    }

    eXosip_event_t *je;

    je = eXosip_event_wait(0, 50);
    eXosip_automatic_action();

    if(je==NULL) {
        eXosip_quit();
        /*DEBUG*/
        /*fprintf(stdout, "[initial registration] no response received");*/
        return -1;
    }else {
        /*DEBUG*/
        /*fprintf(stdout, "[initial registration] Event '%s' received..\n", je->textinfo);*/
        /*fprintf(stdout, "[initial registration] response = '%i'\n", je->response->status_code);*/
    }

    /*DEBUG*/
    /*fprintf(stdout, "[initial registration] ...done\n");*/

    return id;
}

int
sipstack_unregister(int id) {
    osip_message_t *reg = NULL;
    int i;

    /*DEBUG*/
    /*fprintf(stdout, "\n[unregistration] unregister user...\n");*/

    eXosip_lock();
    i = eXosip_register_build_register(id, 0, &reg);

    if (i < 0) {
        eXosip_unlock();
        /*DEBUG*/
        /*fprintf(stderr, "[unregistration] cannot build REGISTER\n");*/
        return -1;
    }
    /*DEBUG*/
    /*fprintf(stdout, "[unregistration] REGISTER build\n");*/

    i = eXosip_register_send_register(id, reg);
    eXosip_unlock();
    if (i < 0) {
        eXosip_quit();
        /*DEBUG*/
        /*fprintf(stdout, "[unregistration] sending REGISTER failed");*/
        return -1;
    }

    eXosip_event_t *je;

    je = eXosip_event_wait(0, 50);
    eXosip_automatic_action();

    if(je==NULL) {
        eXosip_quit();
        /*DEBUG*/
        /*fprintf(stdout, "[unregistration] no response received");*/
        return -1;
    }else {
        /*DEBUG*/
        /*fprintf(stdout, "[unregistration] Event '%s' received..\n", je->textinfo);*/
        /*fprintf(stdout, "[unregistration] response = '%i'\n", je->response->status_code);*/
    }

    /*DEBUG*/
    /*fprintf(stdout, "[unregistration] ...done\n");*/

    return i;
}

int
sipstack_update_register(int id, int expire) {
    osip_message_t *reg = NULL;
    int i;

    /*DEBUG*/
    /*fprintf(stdout, "\n[update registration] update registration of user...\n");*/

    eXosip_lock();
    i = eXosip_register_build_register(id, expire, &reg);

    if (i < 0) {
        eXosip_unlock();
        /*DEBUG*/
        /*fprintf(stderr, "[update registration] cannot build REGISTER\n");*/
        return -1;
    }
    /*DEBUG*/
    /*fprintf(stdout, "[update registration] REGISTER build\n");*/

    i = eXosip_register_send_register(id, reg);
    eXosip_unlock();
    if (i < 0) {
        eXosip_quit();
        /*DEBUG*/
        /*fprintf(stdout, "[update registration] sending REGISTER failed");*/
        return -1;
    }

    eXosip_event_t *je;

    je = eXosip_event_wait(0, 50);
    eXosip_automatic_action();

    if(je==NULL) {
        eXosip_quit();
        /*DEBUG*/
        /*fprintf(stdout, "[update registration] no response received");*/
        return -1;
    }else {
        /*DEBUG*/
        /*fprintf(stdout, "[update registration] Event '%s' received..\n", je->textinfo);*/
        /*fprintf(stdout, "[update registration] response = '%i'\n", je->response->status_code);*/
    }

    /*DEBUG*/
    /*fprintf(stdout, "[update registration] ...done\n");*/

    return i;
}

/**
    start a new call
*/
int
sipstack_start_call(int id) {
    int callID = id;

    return callID;
}

int
sipstack_cancel_call(int callId) {
    return 0;
}

int
sipstack_quit_call(int callId) {
    return 0;
}

int
sipstack_decline_call(int callId) {
    return 0;
}

int
sipstack_set_listener(int listener) {
    return 0;
}
