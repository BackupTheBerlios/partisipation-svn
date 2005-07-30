#include <xmlrpc.h>

xmlrpc_value *make_call_RP(xmlrpc_env * const env,
                           xmlrpc_value * const param_array,
                           void *const server_context);

xmlrpc_value *end_call_RP(xmlrpc_env * const env,
                          xmlrpc_value * const param_array,
                          void *const server_context);

xmlrpc_value *accept_call_RP(xmlrpc_env * const env,
                             xmlrpc_value * const param_array,
                             void *const server_context);
