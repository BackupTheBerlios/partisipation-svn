#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <xmlrpc_client.h>

#include <remote/callback/xmlrpc/xmlrpc_callback_client.h>

#define NAME "Core CallbackClient"
#define VERSION "1.0"

xmlrpc_env callbackEnv;

int cb_init() {
	/*
	 * Initialize our error-handling environment. 
	 */
	xmlrpc_env_init(&callbackEnv);

	/*
	 * Start up our XML-RPC client library. 
	 */
	xmlrpc_client_init2(&callbackEnv, XMLRPC_CLIENT_NO_FLAGS, NAME,
						VERSION, NULL, 0);
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
