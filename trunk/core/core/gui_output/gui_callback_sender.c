#include <stdlib.h>
#include <stdio.h>

#include <remote/callback/gui_callback.h>

int go_change_reg_status(int accountId, int registered) {
	return change_reg_status(accountId, registered);
}

int go_change_call_status(int callId, char *status) {
	return change_call_status(callId, status);
}

int go_show_user_event(int accountId, char *category, char *title,
					   char *message, char *detailMessage) {
	return show_user_event(accountId, category, title, message,
						   detailMessage);
}

int go_register_core() {
	return register_core();
}

int go_incoming_call(int accountId, int callId, char *callerSipUri,
					 char *callerDisplayName) {
	return incoming_call(accountId, callId, callerSipUri,
						 callerDisplayName);
}

int go_set_speaker_volume_cb(double level) {
	return set_speaker_volume_cb(level);
}

int go_set_micro_volume_cb(double level) {
	return set_micro_volume_cb(level);
}
