#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <xmlrpc_client.h>

#include <remote/callback/xmlrpc/xmlrpc_callback_client.h>
#include <util/config/globals.h>

#define XMLRPC_CALLBACK_CLIENT_PREFIX "[xmlrpc callback client] "

xmlrpc_env callbackEnv;

int cb_init() {
	// initialize error-handling environment
	xmlrpc_env_init(&callbackEnv);

	// start up XML-RPC client library
	xmlrpc_client_init2(&callbackEnv, XMLRPC_CLIENT_NO_FLAGS,
						config.remote.callback.xmlrpcClient.name,
						config.remote.callback.xmlrpcClient.version, NULL,
						0);

	if (callbackEnv.fault_occurred) {
		LOG_ERROR(XMLRPC_CALLBACK_CLIENT_PREFIX "XML-RPC Fault occurred "
				  "while trying to initialize XML-RPC client library: "
				  "%s (%d)", callbackEnv.fault_string,
				  callbackEnv.fault_code);
		xmlrpc_env_clean(&callbackEnv);
		return 0;
	}

	return 1;
}

int cb_destroy() {
	// clean up error-handling environment
	xmlrpc_env_clean(&callbackEnv);

	// shutdown XML-RPC client library
	xmlrpc_client_cleanup();

	return 1;
}
