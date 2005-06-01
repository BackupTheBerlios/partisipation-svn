/* A simple standalone XML-RPC server written in C. */

#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <xmlrpc_server.h>
#include <xmlrpc_server_abyss.h>
#include <registration_management.h>
#include <call_management.h>

const int SERVER_PORT = 7777;

int 
main(int           const argc, 
     const char ** const argv) {

    xmlrpc_server_abyss_parms serverparm;
    xmlrpc_registry * registryP;
    xmlrpc_env env;

    xmlrpc_env_init(&env);

    registryP = xmlrpc_registry_new(&env);

    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.registerGui", &register_gui, NULL);
    xmlrpc_registry_add_method(
        &env, registryP, NULL, "sip.makeCall", &make_call, NULL);

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
