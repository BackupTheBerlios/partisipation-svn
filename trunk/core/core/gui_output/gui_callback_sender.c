#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <util/threads/thread_management.h>
#include <util/logging/logger.h>
#include <remote/callback/gui_callback.h>
#include <core/gui_output/gui_callback_sender.h>

/**
 * Logging prefix for GUI callback sender.
 */
#define GUI_CB_SNDR_MSG_PREFIX "[GUI callback sender] "

void *go_change_reg_status_thr(void *args) {
	int rc;
	void **params;
	int accountId;
	int registered;

	params = (void **) args;
	if (!params) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_change_reg_status thread "
				  "received no parameters");
		thread_terminated();
		return NULL;
	}

	accountId = (int) params[0];
	registered = (int) params[1];

	rc = change_reg_status(accountId, registered);
	if (!rc) {
		LOG_MESSAGE(GUI_CB_SNDR_MSG_PREFIX "go_change_reg_status thread "
					"failed to change_reg_status");
		free(params);
		thread_terminated();
		return NULL;
	}

	free(params);
	thread_terminated();
	return NULL;
}

int go_change_reg_status(int accountId, int registered) {
	int rc;						// return code
	void **params;				// parameter array

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "entering go_change_reg_status()");

	params = (void **) malloc(2 * sizeof(void *));
	params[0] = (void *) accountId;
	params[1] = (void *) registered;

	rc = start_thread(go_change_reg_status_thr, (void *) params);
	if (!rc) {
		// ERROR
		LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX
				  "leaving go_change_reg_status(): ERROR");
		return 0;
	}

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX " leaving go_change_reg_status()");
	return 1;					// pseudo success
}

void *go_change_call_status_thr(void *args) {
	int rc;
	void **params;
	int callId;
	char *status;

	params = (void **) args;
	if (!params) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_change_call_status thread "
				  "received no parameters");
		thread_terminated();
		return NULL;
	}

	callId = (int) params[0];
	status = (char *) params[1];

	if (!status) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_change_call_status thread "
				  "received no status parameter");
		free(params);
		thread_terminated();
		return NULL;
	}

	rc = change_call_status(callId, status);
	if (!rc) {
		LOG_MESSAGE(GUI_CB_SNDR_MSG_PREFIX "go_change_reg_status thread "
					"failed to change_reg_status");
		free(status);
		free(params);
		thread_terminated();
		return NULL;
	}

	free(status);
	free(params);
	thread_terminated();
	return NULL;
}

int go_change_call_status(int callId, char *status) {
	int rc;						// return code
	void **params;				// parameter array

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "entering go_change_call_status()");

	params = (void **) malloc(2 * sizeof(void *));
	params[0] = (void *) callId;
	params[1] = (void *) malloc(strlen(status) * sizeof(char) + 1);
	strcpy((char *) params[1], status);

	rc = start_thread(go_change_call_status_thr, (void *) params);
	if (!rc) {
		// ERROR
		LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX
				  "leaving go_change_call_status(): ERROR");
		return 0;
	}

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX " leaving go_change_call_status()");
	return 1;					// pseudo success
}

void *go_show_user_event_thr(void *args) {
	int rc;
	void **params;
	int accountId;
	char *category;
	char *title;
	char *message;
	char *detailMessage;

	params = (void **) args;
	if (!params) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_show_user_event thread "
				  "received no parameters");
		thread_terminated();
		return NULL;
	}

	accountId = (int) params[0];
	category = (char *) params[1];
	title = (char *) params[2];
	message = (char *) params[3];
	detailMessage = (char *) params[4];

	if (!category) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_show_user_event thread "
				  "received no category parameter");
		free(params);
		thread_terminated();
		return NULL;
	}

	if (!title) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_show_user_event thread "
				  "received no title parameter");
		free(category);
		free(params);
		thread_terminated();
		return NULL;
	}

	if (!message) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_show_user_event thread "
				  "received no message parameter");
		free(category);
		free(title);
		free(params);
		thread_terminated();
		return NULL;
	}

	if (!detailMessage) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_show_user_event thread "
				  "received no detailMessage parameter");
		free(category);
		free(title);
		free(message);
		free(params);
		thread_terminated();
		return NULL;
	}

	rc = show_user_event(accountId, category, title, message,
						 detailMessage);
	if (!rc) {
		LOG_MESSAGE(GUI_CB_SNDR_MSG_PREFIX "go_show_user_event thread "
					"failed to show_user_event");
		free(category);
		free(title);
		free(message);
		free(detailMessage);
		free(params);
		thread_terminated();
		return NULL;
	}

	free(category);
	free(title);
	free(message);
	free(detailMessage);
	free(params);
	thread_terminated();
	return NULL;
}

int go_show_user_event(int accountId, char *category,
					   char *title, char *message, char *detailMessage) {
	int rc;						// return code
	void **params;				// parameter array

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "entering go_show_user_event()");

	params = (void **) malloc(5 * sizeof(void *));
	params[0] = (void *) accountId;
	params[1] = (void *) malloc(strlen(category) * sizeof(char) + 1);
	strcpy((char *) params[1], category);
	params[2] = (void *) malloc(strlen(title) * sizeof(char) + 1);
	strcpy((char *) params[2], title);
	params[3] = (void *) malloc(strlen(message) * sizeof(char) + 1);
	strcpy((char *) params[3], message);
	params[4] = (void *) malloc(strlen(detailMessage) * sizeof(char) + 1);
	strcpy((char *) params[4], detailMessage);

	rc = start_thread(go_show_user_event_thr, (void *) params);
	if (!rc) {
		// ERROR
		LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX
				  "leaving go_show_user_event(): ERROR");
		return 0;
	}

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX " leaving go_show_user_event()");
	return 1;					// pseudo success
}

int go_register_core() {
	return register_core();
}

void *go_incoming_call_thr(void *args) {
	int rc;
	void **params;
	int accountId;
	int callId;
	char *callerSipUri;
	char *callerDisplayName;

	params = (void **) args;
	if (!params) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_incoming_call thread "
				  "received no parameters");
		thread_terminated();
		return NULL;
	}

	accountId = (int) params[0];
	callId = (int) params[1];
	callerSipUri = (char *) params[2];
	callerDisplayName = (char *) params[3];

	if (!callerSipUri) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_incoming_call thread "
				  "received no caller URI parameter");
		free(params);
		thread_terminated();
		return NULL;
	}

	if (!callerDisplayName) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_incoming_call thread "
				  "received no caller display name parameter");
		free(callerSipUri);
		free(params);
		thread_terminated();
		return NULL;
	}

	rc = incoming_call(accountId, callId, callerSipUri, callerDisplayName);

	if (!rc) {
		LOG_MESSAGE(GUI_CB_SNDR_MSG_PREFIX "go_incoming_call thread: "
					"incoming_call failed");
		free(callerSipUri);
		free(callerDisplayName);
		free(params);
		thread_terminated();
		return NULL;
	}

	free(callerSipUri);
	free(callerDisplayName);
	free(params);
	thread_terminated();
	return NULL;
}

int go_incoming_call(int accountId, int callId,
					 char *callerSipUri, char *callerDisplayName) {
	int rc;						// return code
	void **params;				// parameter array

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "entering go_incoming_call()");

	params = (void **) malloc(4 * sizeof(void *));
	params[0] = (void *) accountId;
	params[1] = (void *) callId;
	params[2] = (void *) malloc(strlen(callerSipUri) * sizeof(char) + 1);
	strcpy((char *) params[2], callerSipUri);
	params[3] =
		(void *) malloc(strlen(callerDisplayName) * sizeof(char) + 1);
	strcpy((char *) params[3], callerDisplayName);

	rc = start_thread(go_incoming_call_thr, (void *) params);
	if (!rc) {
		// ERROR
		LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX
				  "leaving go_incoming_call(): ERROR");
		return 0;
	}

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX " leaving go_incoming_call()");
	return 1;					// pseudo success
}

void *go_set_speaker_volume_cb_thr(void *args) {
	int rc;
	void **params;
	double *lvlPtr;
	double level;

	params = (void **) args;
	if (!params) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_speaker_volume_cb thread "
				  "received no parameters");
		thread_terminated();
		return NULL;
	}

	lvlPtr = (double *) params[0];
	if (!lvlPtr) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_speaker_volume_cb thread "
				  "received no level parameter");
		free(params);
		thread_terminated();
		return NULL;
	}

	level = *lvlPtr;

	rc = set_speaker_volume_cb(level);
	if (!rc) {
		LOG_MESSAGE(GUI_CB_SNDR_MSG_PREFIX
					"go_set_speaker_volume_cb thread "
					"failed to set_speaker_volume_cb");
		free(lvlPtr);
		free(params);
		thread_terminated();
		return NULL;
	}

	free(lvlPtr);
	free(params);
	thread_terminated();
	return NULL;
}

int go_set_speaker_volume_cb(double level) {
	int rc;						// return code
	void **params;				// parameter array

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX
			  "entering go_set_speaker_volume_cb()");

	params = (void **) malloc(1 * sizeof(void *));
	params[0] = (void *) malloc(1 * sizeof(double));
	*((double *) params[0]) = level;

	rc = start_thread(go_set_speaker_volume_cb_thr, (void *) params);
	if (!rc) {
		// ERROR
		LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX
				  "leaving go_set_speaker_volume_cb(): ERROR");
		return 0;
	}

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX
			  " leaving go_set_speaker_volume_cb()");
	return 1;					// pseudo success
}

void *go_set_micro_volume_cb_thr(void *args) {
	int rc;
	void **params;
	double *lvlPtr;
	double level;

	params = (void **) args;
	if (!params) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_micro_volume_cb thread "
				  "received no parameters");
		thread_terminated();
		return NULL;
	}

	lvlPtr = (double *) params[0];
	if (!lvlPtr) {
		LOG_ERROR(GUI_CB_SNDR_MSG_PREFIX "go_set_micro_volume_cb thread "
				  "received no level parameter");
		free(params);
		thread_terminated();
		return NULL;
	}

	level = *lvlPtr;

	rc = set_micro_volume_cb(level);
	if (!rc) {
		LOG_MESSAGE(GUI_CB_SNDR_MSG_PREFIX
					"go_set_micro_volume_cb thread "
					"failed to set_micro_volume_cb");
		free(lvlPtr);
		free(params);
		thread_terminated();
		return NULL;
	}

	free(lvlPtr);
	free(params);
	thread_terminated();
	return NULL;
}

int go_set_micro_volume_cb(double level) {
	int rc;						// return code
	void **params;				// parameter array

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX "entering go_set_micro_volume_cb()");

	params = (void **) malloc(1 * sizeof(void *));
	params[0] = (void *) malloc(1 * sizeof(double));
	*((double *) params[0]) = level;

	rc = start_thread(go_set_micro_volume_cb_thr, (void *) params);
	if (!rc) {
		// ERROR
		LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX
				  "leaving go_set_micro_volume_cb(): ERROR");
		return 0;
	}

	LOG_DEBUG(GUI_CB_SNDR_MSG_PREFIX " leaving go_set_micro_volume_cb()");
	return 1;					// pseudo success
}
