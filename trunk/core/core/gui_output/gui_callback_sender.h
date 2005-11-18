#ifndef HGUI_CALLBACK_SENDER_USED
#define HGUI_CALLBACK_SENDER_USED

#include <stdlib.h>
#include <stdio.h>

int go_init();

int go_change_reg_status(int accountId, int registered);

int go_change_call_status(int callId, char *status);

int go_show_user_event(int accountId, char *category, char *title,
					   char *message, char *detailMessage);

int go_register_core();

int go_incoming_call(int accountId, int callId, char *callerSipUri,
					 char *callerDisplayName);

int go_set_speaker_volume_cb(double level);

int go_set_micro_volume_cb(double level);

int go_destroy();

#endif
