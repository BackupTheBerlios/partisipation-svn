#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <xmlrpc_client.h>

#include <callback/callback_util.h>

#define NAME "Core CallbackClient"
#define VERSION "1.0"

void die_if_fault_occurred(xmlrpc_env * env) {
	if (env->fault_occurred) {
		fprintf(stderr, "XML-RPC Fault: %s (%d)\n",
				env->fault_string, env->fault_code);
		exit(1);
	}
}

xmlrpc_env callback_init() {
	xmlrpc_env env;

	/*
	 * Initialize our error-handling environment. 
	 */
	xmlrpc_env_init(&env);

	/*
	 * Start up our XML-RPC client library. 
	 */
	xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL,
						0);
	die_if_fault_occurred(&env);

	return env;
}

void callback_cleanup(xmlrpc_env env) {
	/*
	 * Clean up our error-handling environment. 
	 */
	xmlrpc_env_clean(&env);

	/*
	 * Shutdown our XML-RPC client library. 
	 */
	xmlrpc_client_cleanup();
}
