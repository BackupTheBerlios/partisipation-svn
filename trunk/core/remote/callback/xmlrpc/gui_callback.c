#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <xmlrpc_client.h>

#include <remote/callback/xmlrpc/xmlrpc_callback_client.h>
#include <remote/callback/gui_callback.h>
#include <util/config/globals.h>
#include <util/logging/logger.h>

#define GUI_CALLBACK_MSG_PREFIX "[gui callback] "

int xmlrpc_error_occurred(xmlrpc_env * env, const char *methodName) {
	if (env->fault_occurred) {
		LOG_ERROR(GUI_CALLBACK_MSG_PREFIX "XML-RPC Fault occurred while "
				  "trying to call %s method at <%s>: %s (%d)", methodName,
				  config.remote.callback.guiCallback.guiURL,
				  env->fault_string, env->fault_code);

		// clear error from xmlrpc environment:
		xmlrpc_env_clean(env);
		xmlrpc_env_init(env);

		// current URL is no longer valid, prevent other functions from 
		// calling it:
		free(config.remote.callback.guiCallback.guiURL);
		config.remote.callback.guiCallback.guiURL = NULL;
		return 1;
	}
	return 0;
}

int change_reg_status(int accountId, int registered) {
	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.changeRegStatus";

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "entering GUI.changeRegStatus...");

	if (!config.remote.callback.guiCallback.guiURL) {
		LOG_ERROR(GUI_CALLBACK_MSG_PREFIX "GUI URL is not set, can not "
				  "alter registration status");
		return 0;
	}
	// make the remote procedure call
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.guiURL,
						   methodName, "(ib)", (xmlrpc_int32) accountId,
						   (xmlrpc_bool) registered);

	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	xmlrpc_read_bool(&callbackEnv, result, &success);
	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "GUI.changeRegStatus returned "
			  "with %d", success);

	// dispose result value
	xmlrpc_DECREF(result);

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "leaving GUI.changeRegStatus");
	return success;
}

int change_call_status(int callId, char *status) {
	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.changeCallStatus";

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "entering GUI.changeCallStatus...");

	if (!config.remote.callback.guiCallback.guiURL) {
		LOG_ERROR(GUI_CALLBACK_MSG_PREFIX
				  "GUI URL is not set, can not " "alter call status");
		return 0;
	}
	// make the remote procedure call
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.guiURL,
						   methodName, "(is)", (xmlrpc_int32) callId,
						   status);

	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	xmlrpc_read_bool(&callbackEnv, result, &success);
	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "GUI.changeCallStatus returned "
			  "with %d", success);

	// dispose result value
	xmlrpc_DECREF(result);

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "leaving GUI.changeCallStatus...");
	return success;
}

int show_user_event(int accountId, char *category, char *title,
					char *message, char *detailMessage) {

	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.showUserEvent";

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "entering GUI.showUserEvent...");

	if (!config.remote.callback.guiCallback.guiURL) {
		LOG_ERROR(GUI_CALLBACK_MSG_PREFIX
				  "GUI URL is not set, can not " "show user event");
		return 0;
	}
	// make the remote procedure call
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.
						   guiURL, methodName, "(issss)",
						   (xmlrpc_int32) accountId, category, title,
						   message, detailMessage);

	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	xmlrpc_read_bool(&callbackEnv, result, &success);
	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "GUI.showUserEvent returned with %d",
			  success);

	// dispose result value
	xmlrpc_DECREF(result);

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "leaving GUI.showUserEvent");
	return success;
}

int register_core() {
	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.registerCore";

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "entering GUI.registerCore...");

	if (!config.remote.callback.guiCallback.guiURL) {
		LOG_ERROR(GUI_CALLBACK_MSG_PREFIX
				  "GUI URL is not set, can not " "register Core");
		return 0;
	}
	// make the remote procedure call
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.guiURL,
						   methodName, "()", NULL);

	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	xmlrpc_read_bool(&callbackEnv, result, &success);
	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "GUI.registerCore returned with %d",
			  success);

	// dispose result value
	xmlrpc_DECREF(result);

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "leaving GUI.registerCore");
	return success;
}

int incoming_call(int accountId, int callId, char *callerSipUri,
				  char *callerDisplayName) {

	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.incomingCall";

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "entering GUI.incomingCall...");

	if (!config.remote.callback.guiCallback.guiURL) {
		LOG_ERROR(GUI_CALLBACK_MSG_PREFIX
				  "GUI URL is not set, can not " "signal incoming call");
		return 0;
	}
	// make the remote procedure call
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.guiURL,
						   methodName, "(iiss)", (xmlrpc_int32) accountId,
						   (xmlrpc_int32) callId, callerSipUri,
						   callerDisplayName);

	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	xmlrpc_read_bool(&callbackEnv, result, &success);
	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "GUI.incomingCall returned with %d",
			  success);

	// dispose result value
	xmlrpc_DECREF(result);

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "leaving GUI.incomingCall");
	return success;
}

int set_speaker_volume_cb(double level) {
	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.setSpeakerVolume";

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "entering GUI.setSpeakerVolume...");

	if (!config.remote.callback.guiCallback.guiURL) {
		LOG_ERROR(GUI_CALLBACK_MSG_PREFIX "GUI URL is not set, can not "
				  "set speaker volume");
		return 0;
	}
	// make the remote procedure call
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.guiURL,
						   methodName, "(d)", (xmlrpc_double) level);

	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	xmlrpc_read_bool(&callbackEnv, result, &success);
	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "GUI.setSpeakerVolume returned "
			  "with %d", success);

	// dispose result value
	xmlrpc_DECREF(result);

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "leaving GUI.setSpeakerVolume");
	return success;
}

int set_micro_volume_cb(double level) {
	xmlrpc_value *result;
	xmlrpc_bool success;
	const char *methodName = "gui.setMicroVolume";

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "entering GUI.setMicroVolume...");

	if (!config.remote.callback.guiCallback.guiURL) {
		LOG_ERROR(GUI_CALLBACK_MSG_PREFIX "GUI URL is not set, can not "
				  "set microphone volume");
		return 0;
	}
	// make the remote procedure call
	result =
		xmlrpc_client_call(&callbackEnv,
						   config.remote.callback.guiCallback.guiURL,
						   methodName, "(d)", (xmlrpc_double) level);

	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	xmlrpc_read_bool(&callbackEnv, result, &success);
	if (xmlrpc_error_occurred(&callbackEnv, methodName)) {
		return 0;
	}

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "GUI.setMicroVolume returned "
			  "with %d", success);

	// dispose result value
	xmlrpc_DECREF(result);

	LOG_DEBUG(GUI_CALLBACK_MSG_PREFIX "leaving GUI.setMicroVolume");
	return success;
}
