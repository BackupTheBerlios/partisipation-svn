#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <remote/server/xmlrpc/calls_remote.h>
#include <remote/server/calls.h>
#include <util/logging/logger.h>

#define CALLS_REMOTE_MSG_PREFIX "[calls remote]"

xmlrpc_value *make_call_RP(xmlrpc_env * const env,
						   xmlrpc_value * const param_array,
						   void *const server_context) {

	log_message(LOG_DEBUG, CALLS_REMOTE_MSG_PREFIX "entering makeCall...");

	xmlrpc_int32 accountId;
	char *callee;
	int result;

	xmlrpc_decompose_value(env, param_array, "(is)", &accountId, &callee);
	if (env->fault_occurred) {
		return NULL;
	}

	result = make_call(accountId, callee);

	log_message(LOG_DEBUG, CALLS_REMOTE_MSG_PREFIX "leaving makeCall");
	return xmlrpc_build_value(env, "i", result);
}

xmlrpc_value *end_call_RP(xmlrpc_env * const env,
						  xmlrpc_value * const param_array,
						  void *const server_context) {
	log_message(LOG_DEBUG, CALLS_REMOTE_MSG_PREFIX "entering endCall...");

	xmlrpc_int32 callId;
	int result;

	xmlrpc_decompose_value(env, param_array, "(i)", &callId);
	if (env->fault_occurred) {
		return NULL;
	}

	result = end_call(callId);

	log_message(LOG_DEBUG, CALLS_REMOTE_MSG_PREFIX "leaving endCall");
	return xmlrpc_build_value(env, "b", result);
}

xmlrpc_value *accept_call_RP(xmlrpc_env * const env,
							 xmlrpc_value * const param_array,
							 void *const server_context) {
	log_message(LOG_DEBUG,
				CALLS_REMOTE_MSG_PREFIX "entering acceptCall...");

	xmlrpc_int32 callId;
	int result;

	xmlrpc_decompose_value(env, param_array, "(i)", &callId);
	if (env->fault_occurred) {
		return NULL;
	}

	result = accept_call(callId);

	log_message(LOG_DEBUG, CALLS_REMOTE_MSG_PREFIX "leaving acceptCall");
	return xmlrpc_build_value(env, "b", result);
}
