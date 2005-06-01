#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <call_management.h>

xmlrpc_value *
make_call(xmlrpc_env *   const env, 
          xmlrpc_value * const param_array, 
          void *         const server_context) {
          
    printf("entering makeCall...\n");
    
    xmlrpc_int32 accountId;
    char* callee;
    
    xmlrpc_decompose_value(env, param_array, "(is)", &accountId, &callee);
    if (env->fault_occurred) {
        return NULL;
    }

    // do something
    
    printf("accountId: %d\n", accountId);
    printf("callee: %s\n", callee);
        
    printf("leaving makeCall\n");
    return xmlrpc_build_value(env, "i", 4711);
}

xmlrpc_value *
end_call(xmlrpc_env *   const env, 
         xmlrpc_value * const param_array, 
         void *         const server_context) {
          
    printf("entering endCall...\n");
    
    xmlrpc_int32 callId;
    
    xmlrpc_decompose_value(env, param_array, "(i)", &callId);
    if (env->fault_occurred) {
        return NULL;
    }

    // do something
    
    printf("callId: %d\n", callId);
        
    printf("leaving endCall\n");
    return xmlrpc_build_value(env, "b", 1);
}

xmlrpc_value *
accept_call(xmlrpc_env *   const env, 
            xmlrpc_value * const param_array, 
            void *         const server_context) {
          
    printf("entering acceptCall...\n");
    
    xmlrpc_int32 callId;
    
    xmlrpc_decompose_value(env, param_array, "(i)", &callId);
    if (env->fault_occurred) {
        return NULL;
    }

    // do something
    
    printf("callId: %d\n", callId);
        
    printf("leaving acceptCall\n");
    return xmlrpc_build_value(env, "b", 1);
}
