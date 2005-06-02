#include <xmlrpc.h>

xmlrpc_value *
account_get_all(xmlrpc_env *   const env, 
                xmlrpc_value * const param_array, 
                void *         const server_context);

xmlrpc_value *
account_set(xmlrpc_env *   const env, 
            xmlrpc_value * const param_array, 
            void *         const server_context);
            
xmlrpc_value *
account_get(xmlrpc_env *   const env, 
            xmlrpc_value * const param_array, 
            void *         const server_context);
            
xmlrpc_value *
account_create(xmlrpc_env *   const env, 
               xmlrpc_value * const param_array, 
               void *         const server_context);
               
xmlrpc_value *
account_delete(xmlrpc_env *   const env, 
               xmlrpc_value * const param_array, 
               void *         const server_context);

xmlrpc_value *
account_register(xmlrpc_env *   const env, 
         xmlrpc_value * const param_array, 
         void *         const server_context);

xmlrpc_value *
account_unregister(xmlrpc_env *   const env, 
                   xmlrpc_value * const param_array, 
                   void *         const server_context);
