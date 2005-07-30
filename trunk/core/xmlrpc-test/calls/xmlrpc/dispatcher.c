#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <calls/xmlrpc/dispatcher.h>
#include <calls/call_management.h>

xmlrpc_value *
make_call_RP(xmlrpc_env * const env,
             xmlrpc_value * const param_array, void *const server_context) {

    printf("entering makeCall...\n");

    xmlrpc_int32 accountId;
    char *callee;
    int result;

    xmlrpc_decompose_value(env, param_array, "(is)", &accountId, &callee);
    if (env->fault_occurred) {
        return NULL;
    }

    result = make_call(accountId, callee);

    printf("leaving makeCall\n");
    return xmlrpc_build_value(env, "i", result);
}

xmlrpc_value *
end_call_RP(xmlrpc_env * const env,
            xmlrpc_value * const param_array, void *const server_context) {

    printf("entering endCall...\n");

    xmlrpc_int32 callId;
    int result;

    xmlrpc_decompose_value(env, param_array, "(i)", &callId);
    if (env->fault_occurred) {
        return NULL;
    }

    result = end_call(callId);

    printf("leaving endCall\n");
    return xmlrpc_build_value(env, "b", result);
}

xmlrpc_value *
accept_call_RP(xmlrpc_env * const env,
               xmlrpc_value * const param_array, void *const server_context) {

    printf("entering acceptCall...\n");

    xmlrpc_int32 callId;
    int result;

    xmlrpc_decompose_value(env, param_array, "(i)", &callId);
    if (env->fault_occurred) {
        return NULL;
    }

    result = accept_call(callId);

    printf("leaving acceptCall\n");
    return xmlrpc_build_value(env, "b", result);
}
