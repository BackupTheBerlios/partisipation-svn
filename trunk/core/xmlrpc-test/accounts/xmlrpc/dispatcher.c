#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <accounts/xmlrpc/dispatcher.h>
#include <accounts/account_management.h>


xmlrpc_value *
account_get_all_RP(xmlrpc_env *   const env, 
                   xmlrpc_value * const param_array, 
                   void *         const server_context) {
          
    printf("entering accountGetAll...\n");
    
    int * ids;
    account_get_all(ids);
    
    int len = sizeof(ids) / sizeof(int);
    int result[len];
    
    int i;
    for (i = 0; i < len; i++) {
        result[i] = ids[i];
    }
    free(ids);
    
    printf("leaving accountGetAll\n");
    return xmlrpc_build_value(env, "A", result);
}

xmlrpc_value *
account_set_RP(xmlrpc_env *   const env, 
               xmlrpc_value * const param_array, 
               void *         const server_context) {
          
    printf("entering accountSet...\n");
    
    xmlrpc_int32 accountId;
    char* attribute;
    char* value;
    int result;
    
    xmlrpc_decompose_value(env, param_array, "(iss)", &accountId, &attribute, &value);
    if (env->fault_occurred) {
        return NULL;
    }
    
    result = account_set(accountId, attribute, value);    
        
    printf("leaving accountSet\n");
    return xmlrpc_build_value(env, "b", result);
}
            
xmlrpc_value *
account_get_RP(xmlrpc_env *   const env, 
               xmlrpc_value * const param_array, 
               void *         const server_context) {
          
    printf("entering accountGet...\n");
    
    xmlrpc_int32 accountId;
    char* attribute;
    char* result;
    
    xmlrpc_decompose_value(env, param_array, "(is)", &accountId, &attribute);
    if (env->fault_occurred) {
        return NULL;
    }

    result = account_get(accountId, attribute);
        
    printf("leaving accountGet\n");
    return xmlrpc_build_value(env, "s", result);
}
            
xmlrpc_value *
account_create_RP(xmlrpc_env *   const env, 
                  xmlrpc_value * const param_array, 
                  void *         const server_context) {
          
    printf("entering accountCreate...\n");
    
    int result;
    
    result = account_create();
    
    printf("leaving accountCreate\n");
    return xmlrpc_build_value(env, "i", result);
}
               
xmlrpc_value *
account_delete_RP(xmlrpc_env *   const env, 
                  xmlrpc_value * const param_array, 
                  void *         const server_context) {
          
    printf("entering accountDelete...\n");
    
    xmlrpc_int32 accountId;
    int result;
    
    xmlrpc_decompose_value(env, param_array, "(i)", &accountId);
    if (env->fault_occurred) {
        return NULL;
    }

    result = account_delete(accountId);
        
    printf("leaving accountDelete\n");
    return xmlrpc_build_value(env, "b", result);
}

xmlrpc_value *
account_register_RP(xmlrpc_env *   const env, 
                    xmlrpc_value * const param_array, 
                    void *         const server_context) {
          
    printf("entering register...\n");
    
    xmlrpc_int32 accountId;
    int result;
    
    xmlrpc_decompose_value(env, param_array, "(i)", &accountId);
    if (env->fault_occurred) {
        return NULL;
    }

    result = account_register(accountId);
        
    printf("leaving register\n");
    return xmlrpc_build_value(env, "b", result);
}

xmlrpc_value *
account_unregister_RP(xmlrpc_env *   const env, 
                      xmlrpc_value * const param_array, 
                      void *         const server_context) {
          
    printf("entering unregister...\n");
    
    xmlrpc_int32 accountId;
    int result;
    
    xmlrpc_decompose_value(env, param_array, "(i)", &accountId);
    if (env->fault_occurred) {
        return NULL;
    }

    result = account_unregister(accountId);
        
    printf("leaving unregister\n");
    return xmlrpc_build_value(env, "b", result);
}
