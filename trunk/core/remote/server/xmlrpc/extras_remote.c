#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <remote/server/xmlrpc/extras_remote.h>
#include <remote/server/extras.h>
#include <util/logging/logger.h>

#define EXTRAS_REMOTE_MSG_PREFIX "[extras remote]"

xmlrpc_value *send_dtmf_RP(xmlrpc_env * const env,
						   xmlrpc_value * const param_array,
						   void *const server_context) {

	log_message(LOG_DEBUG,
				EXTRAS_REMOTE_MSG_PREFIX "entering sendDtmf...");

	char *character;
	xmlrpc_int32 callId;
	int result;

	xmlrpc_decompose_value(env, param_array, "(si)", &character, &callId);
	if (env->fault_occurred) {
		return NULL;
	}

	result = send_dtmf(character, callId);

	return xmlrpc_build_value(env, "b", result);
}
