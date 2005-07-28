#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

#include <eXosip2/eXosip.h>
#include "sip_stack_interface.h"

int sipstack_init() {
    /*init eXosip2 and osip*/
    int i;

    TRACE_INITIALIZE (6, stdout);

    i = eXosip_init();
    if(i!=0) {
        fprintf(stderr, "\ncould not initialize eXosip\n");
        return -1;
    }

    //i = eXosip_listen_to(IPPROTO_UDP, INADDR_ANY, 5060);
    i = eXosip_listen_addr(IPPROTO_UDP, INADDR_ANY, 5065, AF_INET, 0);
    if(i!=0) {
        eXosip_quit();
        fprintf(stderr, "could not initialize transport layer\n");
        return -1;
    }
}

/**
    send initial register
    @param char* identity
    @param char* registrar
    @param int expire
*/
int sipstack_send_register(char* const identity, char* const registrar, int expire) {
    osip_message_t *reg = NULL;
    int id;
    int i;

    eXosip_lock();
    id = eXosip_register_build_initial_register(identity, registrar, NULL, expire, &reg);

    if(id < 0) {
        eXosip_unlock();
        return -1;
    }

    osip_message_set_supported(reg, "100rel");
    osip_message_set_supported(reg, "path");

    i = eXosip_register_send_register(id, reg);
    eXosip_unlock();
    return id;
}

int sipstack_unregister(int id) {
    osip_message_t *reg = NULL;
    int i;

    eXosip_lock();
    i = eXosip_register_build_register(id, 0 , &reg);

    if(i < 0) {
        eXosip_unlock();
        return -1;
    }
    i = eXosip_register_send_register(id, reg);
    eXosip_unlock();
    return i;
}

int sipstack_update_register(int id, int expire) {
    osip_message_t *reg = NULL;
    int i;

    eXosip_lock();
    i = eXosip_register_build_register(id, expire, &reg);

    if(i < 0) {
        eXosip_unlock();
        fprintf(stdout, "cannot build REGISTER\n");
        return -1;
    }
    i = eXosip_register_send_register(id, reg);
    eXosip_unlock();
    return i;
}

/**
    start a new call
*/
int sipstack_start_call(int id) {
    int callID = id;
    return callID;
}

int sipstack_cancel_call(int callId) {
    return 0;
}

int sipstack_quit_call(int callId) {
    return 0;
}

int sipstack_decline_call(int callId) {
    return 0;
}

int sipstack_set_listener(int listener) {
    return 0;
}
