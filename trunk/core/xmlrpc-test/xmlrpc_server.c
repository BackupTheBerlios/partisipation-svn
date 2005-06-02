/* A simple standalone XML-RPC server written in C. */

#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <xmlrpc_server.h>
#include <xmlrpc_server_abyss.h>
#include <registration_management.h>
#include <call_management.h>
#include <account_management.h>
#include <volume_management.h>
#include <extras_management.h>

const int SERVER_PORT = 7777;

int 
main(int           const argc, 
     const char ** const argv) {

    xmlrpc_server_abyss_parms serverparm;
    xmlrpc_registry * registryP;
    xmlrpc_env env;

    xmlrpc_env_init(&env);

    registryP = xmlrpc_registry_new(&env);
    
    // registration management:
    
    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.registerGui", &register_gui, NULL);
        
    // call management:
    
    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.makeCall", &make_call, NULL);

    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.endCall", &end_call, NULL);

    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.acceptCall", &accept_call, NULL);
        
    // account management:

    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.accountGetAll", &account_get_all, NULL);

    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.accountSet", &account_set, NULL);

    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.accountGet", &account_get, NULL);

    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.accountCreate", &account_create, NULL);

    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.accountDelete", &account_delete, NULL);

    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.register", &account_register, NULL);

    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.unregister", &account_unregister, NULL);
        
    // volume management:
    
    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.setSpeakerVolume", &set_speaker_volume, NULL);

    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.getSpeakerVolume", &get_speaker_volume, NULL);
    
    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.setMicroVolume", &set_micro_volume, NULL);

    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.getMicroVolume", &get_micro_volume, NULL);
        
    // extras management:
    
    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.sendDtmf", &send_dtmf, NULL);

        
    /* In the modern form of the Abyss API, we supply parameters in memory
       like a normal API.  We select the modern form by setting
       config_file_name to NULL: 
    */
    serverparm.config_file_name = NULL;
    serverparm.registryP = registryP;
    serverparm.port_number = SERVER_PORT;
    serverparm.log_file_name = "/tmp/xmlrpc_log";

    printf("Running XML-RPC server...\n");

    xmlrpc_server_abyss(&env, &serverparm, XMLRPC_APSIZE(log_file_name));

    /* xmlrpc_server_abyss() never returns */

    return 0;
}
