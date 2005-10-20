#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <xmlrpc_client.h>

#include <remote/callback/xmlrpc/xmlrpc_callback_client.h>
#include <remote/callback/gui_callback.h>
#include <util/config/globals.h>

void die_if_fault_occurred(xmlrpc_env * env) {
	if (env->fault_occurred) {
		fprintf(stderr, "XML-RPC Fault: %s (%d)\n",
				env->fault_string, env->fault_code);
		exit(1);
	}
}

int change_reg_status(int accountId, int registered) {

	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.changeRegStatus";

	printf("entering changeRegStatus...\n");

	/*
	 * Make the remote procedure call 
	 */
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.guiURL,
						   methodName, "(ib)", (xmlrpc_int32) accountId,
						   (xmlrpc_bool) registered);
	die_if_fault_occurred(&callbackEnv);

	xmlrpc_read_bool(&callbackEnv, result, &success);
	die_if_fault_occurred(&callbackEnv);
	printf("return %d\n", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);

	printf("leaving changeRegStatus\n");

	return success;
}

int change_call_status(int callId, char *status) {

	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.changeCallStatus";

	printf("entering changeCallStatus...\n");

	/*
	 * Make the remote procedure call 
	 */
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.guiURL,
						   methodName, "(is)", (xmlrpc_int32) callId,
						   status);
	die_if_fault_occurred(&callbackEnv);

	xmlrpc_read_bool(&callbackEnv, result, &success);
	die_if_fault_occurred(&callbackEnv);
	printf("return %d\n", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);

	printf("leaving changeCallStatus...\n");

	return success;
}

int
show_user_event(int accountId,
				char *category,
				char *title, char *message, char *detailMessage) {

	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.showUserEvent";

	printf("entering showUserEvent...\n");

	/*
	 * Make the remote procedure call 
	 */
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.guiURL,
						   methodName, "(issss)", (xmlrpc_int32) accountId,
						   category, title, message, detailMessage);
	die_if_fault_occurred(&callbackEnv);

	xmlrpc_read_bool(&callbackEnv, result, &success);
	die_if_fault_occurred(&callbackEnv);
	printf("return %d\n", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);

	printf("leaving showUserEvent\n");

	return success;
}

int register_core() {
	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.registerCore";

	printf("entering registerCore...\n");

	/*
	 * Make the remote procedure call 
	 */
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.guiURL,
						   methodName, "()", NULL);
	die_if_fault_occurred(&callbackEnv);

	xmlrpc_read_bool(&callbackEnv, result, &success);
	die_if_fault_occurred(&callbackEnv);
	printf("return %d\n", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);

	printf("leaving registerCore\n");

	return success;
}

int
incoming_call(int accountId,
			  int callId, char *callerSipUri, char *callerDisplayName) {
	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.incomingCall";

	printf("entering incomingCall...\n");

	/*
	 * Make the remote procedure call 
	 */
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.guiURL,
						   methodName, "(iiss)", (xmlrpc_int32) accountId,
						   (xmlrpc_int32) callId, callerSipUri,
						   callerDisplayName);
	die_if_fault_occurred(&callbackEnv);

	xmlrpc_read_bool(&callbackEnv, result, &success);
	die_if_fault_occurred(&callbackEnv);
	printf("return %d\n", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);

	printf("leaving incomingCall\n");

	return success;
}

int set_speaker_volume_cb(double level) {
	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.setSpeakerVolume";

	printf("entering setSpeakerVolume...\n");

	/*
	 * Make the remote procedure call 
	 */
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.guiURL,
						   methodName, "(d)", (xmlrpc_double) level);
	die_if_fault_occurred(&callbackEnv);

	xmlrpc_read_bool(&callbackEnv, result, &success);
	die_if_fault_occurred(&callbackEnv);
	printf("return %d\n", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);

	printf("leaving setSpeakerVolume\n");

	return success;
}

int set_micro_volume_cb(double level) {
	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.setMicroVolume";

	printf("entering setMicroVolume...\n");

	/*
	 * Make the remote procedure call 
	 */
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.guiURL,
						   methodName, "(d)", (xmlrpc_double) level);
	die_if_fault_occurred(&callbackEnv);

	xmlrpc_read_bool(&callbackEnv, result, &success);
	die_if_fault_occurred(&callbackEnv);
	printf("return %d\n", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);

	printf("leaving setMicroVolume\n");

	return success;
}
