#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <remote/server/xmlrpc/registration_remote.h>
#include <remote/server/registration.h>
#include <util/logging/logger.h>

#define REGISTRATION_REMOTE_MSG_PREFIX "[registration remote]"

xmlrpc_value *register_gui_RP(xmlrpc_env * const env,
							  xmlrpc_value * const param_array,
							  void *const server_context) {

	char *address;
	xmlrpc_int32 port;
	char *result;

	xmlrpc_decompose_value(env, param_array, "(si)", &address, &port);
	if (env->fault_occurred) {
		return NULL;
	}

	log_message(LOG_INFO,
				REGISTRATION_REMOTE_MSG_PREFIX "register_gui called");

	result = register_gui(address, port);

	return xmlrpc_build_value(env, "s", result);
}

xmlrpc_value *unregister_gui_RP(xmlrpc_env * const env,
								xmlrpc_value * const param_array,
								void *const server_context) {

	char *address;
	xmlrpc_int32 port;
	int result;

	xmlrpc_decompose_value(env, param_array, "(si)", &address, &port);
	if (env->fault_occurred) {
		return NULL;
	}

	log_message(LOG_INFO,
				REGISTRATION_REMOTE_MSG_PREFIX "unregister_gui called");

	result = unregister_gui(address, port);

	return xmlrpc_build_value(env, "b", result);
}
