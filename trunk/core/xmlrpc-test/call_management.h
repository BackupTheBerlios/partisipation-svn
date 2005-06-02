#include <xmlrpc.h>

xmlrpc_value *
make_call(xmlrpc_env *   const env, 
          xmlrpc_value * const param_array, 
          void *         const server_context);

xmlrpc_value *
end_call(xmlrpc_env *   const env, 
         xmlrpc_value * const param_array, 
         void *         const server_context);

xmlrpc_value *
accept_call(xmlrpc_env *   const env, 
            xmlrpc_value * const param_array, 
            void *         const server_context);
