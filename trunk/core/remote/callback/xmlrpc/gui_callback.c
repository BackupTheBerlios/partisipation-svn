#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <xmlrpc_client.h>

#include <remote/callback/xmlrpc/xmlrpc_callback_client.h>
#include <remote/callback/gui_callback.h>
#include <util/config/globals.h>
#include <util/logging/logger.h>

#define GUI_CALLBACK_MSG_PREFIX "[gui callback]"

void die_if_fault_occurred(xmlrpc_env * env) {
	if (env->fault_occurred) {
		log_message(LOG_INFO,
					GUI_CALLBACK_MSG_PREFIX
					"XML-RPC Fault: %s (%d)",
					env->fault_string, env->fault_code);
		exit(1);
	}
}

int change_reg_status(int accountId, int registered) {

	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.changeRegStatus";
	log_message(LOG_INFO,
				GUI_CALLBACK_MSG_PREFIX "entering changeRegStatus...");

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
	log_message(LOG_INFO, GUI_CALLBACK_MSG_PREFIX "return %d", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);

	log_message(LOG_INFO,
				GUI_CALLBACK_MSG_PREFIX "leaving changeRegStatus");

	return success;
}

int change_call_status(int callId, char *status) {

	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.changeCallStatus";

	log_message(LOG_INFO,
				GUI_CALLBACK_MSG_PREFIX "entering changeCallStatus...");

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
	log_message(LOG_INFO, GUI_CALLBACK_MSG_PREFIX "return %d", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);
	log_message(LOG_INFO,
				GUI_CALLBACK_MSG_PREFIX "leaving changeCallStatus...");

	return success;
}

int
show_user_event(int accountId,
				char *category,
				char *title, char *message, char *detailMessage) {

	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.showUserEvent";
	log_message(LOG_INFO,
				GUI_CALLBACK_MSG_PREFIX "entering showUserEvent...");

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
	log_message(LOG_INFO, GUI_CALLBACK_MSG_PREFIX "return %d", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);

	log_message(LOG_INFO, GUI_CALLBACK_MSG_PREFIX "leaving showUserEvent");

	return success;
}

int register_core() {
	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.registerCore";

	log_message(LOG_INFO,
				GUI_CALLBACK_MSG_PREFIX "entering registerCore...");

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
	log_message(LOG_INFO, GUI_CALLBACK_MSG_PREFIX "return %d\n", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);
	log_message(LOG_INFO, GUI_CALLBACK_MSG_PREFIX "leaving registerCore");

	return success;
}

int
incoming_call(int accountId,
			  int callId, char *callerSipUri, char *callerDisplayName) {
	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.incomingCall";

	log_message(LOG_INFO,
				GUI_CALLBACK_MSG_PREFIX "entering incomingCall...");

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
	log_message(LOG_INFO, GUI_CALLBACK_MSG_PREFIX "return %d", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);

	log_message(LOG_INFO, GUI_CALLBACK_MSG_PREFIX "leaving incomingCall");

	return success;
}

int set_speaker_volume_cb(double level) {
	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.setSpeakerVolume";
	log_message(LOG_INFO,
				GUI_CALLBACK_MSG_PREFIX "entering setSpeakerVolume...");

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
	log_message(LOG_INFO, GUI_CALLBACK_MSG_PREFIX "return %d", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);
	log_message(LOG_INFO,
				GUI_CALLBACK_MSG_PREFIX "leaving setSpeakerVolume");

	return success;
}

int set_micro_volume_cb(double level) {
	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.setMicroVolume";

	log_message(LOG_INFO,
				GUI_CALLBACK_MSG_PREFIX "entering setMicroVolume...");

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
	log_message(LOG_INFO, GUI_CALLBACK_MSG_PREFIX "return %d", success);

	/*
	 * Dispose of our result value. 
	 */
	xmlrpc_DECREF(result);

	log_message(LOG_INFO,
				GUI_CALLBACK_MSG_PREFIX "leaving setMicroVolume");

	return success;
}
