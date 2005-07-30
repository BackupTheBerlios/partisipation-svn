#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <registration/xmlrpc/dispatcher.h>
#include <registration/registration_management.h>

xmlrpc_value *
register_gui_RP(xmlrpc_env * const env,
                xmlrpc_value * const param_array, void *const server_context) {

    char *address;
    xmlrpc_int32 port;
    char *result;

    xmlrpc_decompose_value(env, param_array, "(si)", &address, &port);
    if (env->fault_occurred) {
        return NULL;
    }

    printf("register_gui called\n");

    result = register_gui(address, port);

    return xmlrpc_build_value(env, "s", result);
}
