#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <registration_management.h>

xmlrpc_value *
register_gui(xmlrpc_env *   const env, 
             xmlrpc_value * const param_array, 
             void *         const server_context) {
    
    char* address;
    xmlrpc_int32 port;
    
    xmlrpc_decompose_value(env, param_array, "(si)", &address, &port);
    if (env->fault_occurred) {
        return NULL;
    }
    
    printf("register_gui called\n");
    
    // do something
    
    return xmlrpc_build_value(env, "s", "OK");
}
