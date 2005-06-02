#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <account_management.h>


xmlrpc_value *
account_get_all(xmlrpc_env *   const env, 
                xmlrpc_value * const param_array, 
                void *         const server_context) {
          
    printf("entering accountGetAll...\n");
    
    // do something
    
    xmlrpc_int32 result[3];
    result[0] = 1;
    result[1] = 2;
    result[2] = 3;
    
    printf("leaving accountGetAll\n");
    return xmlrpc_build_value(env, "A", result);
}

xmlrpc_value *
account_set(xmlrpc_env *   const env, 
            xmlrpc_value * const param_array, 
            void *         const server_context) {
          
    printf("entering accountSet...\n");
    
    xmlrpc_int32 accountId;
    char* attribute;
    char* value;
    
    xmlrpc_decompose_value(env, param_array, "(iss)", &accountId, &attribute, &value);
    if (env->fault_occurred) {
        return NULL;
    }

    // do something
    
    printf("accountId: %d\n", accountId);
    printf("attribute: %s\n", attribute);
    printf("value: %s\n", value);
        
    printf("leaving accountSet\n");
    return xmlrpc_build_value(env, "b", 1);
}
            
xmlrpc_value *
account_get(xmlrpc_env *   const env, 
            xmlrpc_value * const param_array, 
            void *         const server_context) {
          
    printf("entering accountGet...\n");
    
    xmlrpc_int32 accountId;
    char* attribute;
    
    xmlrpc_decompose_value(env, param_array, "(is)", &accountId, &attribute);
    if (env->fault_occurred) {
        return NULL;
    }

    // do something
    
    printf("accountId: %d\n", accountId);
    printf("attribute: %s\n", attribute);
        
    printf("leaving accountGet\n");
    return xmlrpc_build_value(env, "s", "value");
}
            
xmlrpc_value *
account_create(xmlrpc_env *   const env, 
               xmlrpc_value * const param_array, 
               void *         const server_context) {
          
    printf("entering accountCreate...\n");
    
    // do something
    
    printf("leaving accountCreate\n");
    return xmlrpc_build_value(env, "i", 7);
}
               
xmlrpc_value *
account_delete(xmlrpc_env *   const env, 
               xmlrpc_value * const param_array, 
               void *         const server_context) {
          
    printf("entering accountDelete...\n");
    
    xmlrpc_int32 accountId;
    
    xmlrpc_decompose_value(env, param_array, "(i)", &accountId);
    if (env->fault_occurred) {
        return NULL;
    }

    // do something
    
    printf("accountId: %d\n", accountId);
        
    printf("leaving accountDelete\n");
    return xmlrpc_build_value(env, "b", 1);
}

xmlrpc_value *
account_register(xmlrpc_env *   const env, 
         xmlrpc_value * const param_array, 
         void *         const server_context) {
          
    printf("entering register...\n");
    
    xmlrpc_int32 accountId;
    
    xmlrpc_decompose_value(env, param_array, "(i)", &accountId);
    if (env->fault_occurred) {
        return NULL;
    }

    // do something
    
    printf("accountId: %d\n", accountId);
        
    printf("leaving register\n");
    return xmlrpc_build_value(env, "b", 1);
}

xmlrpc_value *
account_unregister(xmlrpc_env *   const env, 
           xmlrpc_value * const param_array, 
           void *         const server_context) {
          
    printf("entering unregister...\n");
    
    xmlrpc_int32 accountId;
    
    xmlrpc_decompose_value(env, param_array, "(i)", &accountId);
    if (env->fault_occurred) {
        return NULL;
    }

    // do something
    
    printf("accountId: %d\n", accountId);
        
    printf("leaving unregister\n");
    return xmlrpc_build_value(env, "b", 1);
}
