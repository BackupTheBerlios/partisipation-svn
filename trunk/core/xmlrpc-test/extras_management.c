#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <extras_management.h>

xmlrpc_value *
send_dtmf(xmlrpc_env *   const env, 
          xmlrpc_value * const param_array, 
          void *         const server_context) {
          
    printf("entering sendDtmf...\n");

    char* character;
    xmlrpc_int32 callId;
    
    xmlrpc_decompose_value(env, param_array, "(si)", &character, &callId);
    if (env->fault_occurred) {
        return NULL;
    }

    // do something
    
    printf("char: %s\n", character);
    printf("callId: %d\n", callId);
        
    printf("leaving sendDtmf\n");
    return xmlrpc_build_value(env, "b", 1);
}
