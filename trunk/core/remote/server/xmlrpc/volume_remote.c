#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <remote/server/xmlrpc/volume_remote.h>
#include <remote/server/volume.h>
#include <util/logging/logger.h>

#define VOLUME_REMOTE_MSG_PREFIX "[volume remote]"

xmlrpc_value *set_speaker_volume_RP(xmlrpc_env * const env,
									xmlrpc_value * const param_array,
									void *const server_context) {
	log_message(LOG_DEBUG,
				VOLUME_REMOTE_MSG_PREFIX "entering setSpeakerVolume...");

	xmlrpc_double level;
	int result;

	xmlrpc_decompose_value(env, param_array, "(d)", &level);
	if (env->fault_occurred) {
		return NULL;
	}

	result = set_speaker_volume(level);

	log_message(LOG_DEBUG,
				VOLUME_REMOTE_MSG_PREFIX "leaving setSpeakerVolume");
	return xmlrpc_build_value(env, "b", result);
}

xmlrpc_value *get_speaker_volume_RP(xmlrpc_env * const env,
									xmlrpc_value * const param_array,
									void *const server_context) {

	log_message(LOG_DEBUG,
				VOLUME_REMOTE_MSG_PREFIX "entering getSpeakerVolume...");

	xmlrpc_double result;

	result = get_speaker_volume();
	log_message(LOG_DEBUG,
				VOLUME_REMOTE_MSG_PREFIX "leaving getSpeakerVolume");

	return xmlrpc_build_value(env, "d", result);
}

xmlrpc_value *set_micro_volume_RP(xmlrpc_env * const env,
								  xmlrpc_value * const param_array,
								  void *const server_context) {
	log_message(LOG_DEBUG,
				VOLUME_REMOTE_MSG_PREFIX "entering setMicroVolume...");

	xmlrpc_double level;
	int result;

	xmlrpc_decompose_value(env, param_array, "(d)", &level);
	if (env->fault_occurred) {
		return NULL;
	}

	result = set_micro_volume(level);

	log_message(LOG_DEBUG,
				VOLUME_REMOTE_MSG_PREFIX "leaving setMicroVolume");
	return xmlrpc_build_value(env, "b", result);
}

xmlrpc_value *get_micro_volume_RP(xmlrpc_env * const env,
								  xmlrpc_value * const param_array,
								  void *const server_context) {
	log_message(LOG_DEBUG,
				VOLUME_REMOTE_MSG_PREFIX "entering getMicroVolume...");

	xmlrpc_double result;

	result = get_micro_volume();

	log_message(LOG_DEBUG,
				VOLUME_REMOTE_MSG_PREFIX "leaving getMicroVolume");
	return xmlrpc_build_value(env, "d", result);
}
