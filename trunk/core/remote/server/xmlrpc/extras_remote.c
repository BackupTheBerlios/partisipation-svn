#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <remote/server/xmlrpc/extras_remote.h>
#include <remote/server/extras.h>

xmlrpc_value *send_dtmf_RP(xmlrpc_env * const env,
						   xmlrpc_value * const param_array,
						   void *const server_context) {

	printf("entering sendDtmf...\n");

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
