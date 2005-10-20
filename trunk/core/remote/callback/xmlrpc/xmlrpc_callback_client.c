#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <xmlrpc_client.h>

#include <remote/callback/xmlrpc/xmlrpc_callback_client.h>
#include <util/config/globals.h>

xmlrpc_env callbackEnv;

int cb_init() {
	/*
	 * Initialize our error-handling environment. 
	 */
	xmlrpc_env_init(&callbackEnv);

	/*
	 * Start up our XML-RPC client library. 
	 */
	xmlrpc_client_init2(&callbackEnv, XMLRPC_CLIENT_NO_FLAGS,
						config.remote.callback.xmlrpcClient.name,
						config.remote.callback.xmlrpcClient.version, NULL,
						0);
	die_if_fault_occurred(&callbackEnv);

	return 1;
}

int cb_destroy() {
	/*
	 * Clean up our error-handling environment. 
	 */
	xmlrpc_env_clean(&callbackEnv);

	/*
	 * Shutdown our XML-RPC client library. 
	 */
	xmlrpc_client_cleanup();

	return 1;
}
