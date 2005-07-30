#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>

extern xmlrpc_env callbackEnv;

int change_reg_status(int accountId, int registered);

int change_call_status(int callId, char *status);

int
show_user_event(int accountId,
                char *category,
                char *title, char *message, char *detailMessage);

int register_core();

int
incoming_call(int accountId,
              int callId, char *callerSipUri, char *callerDisplayName);

int set_speaker_volume_cb(double level);

int set_micro_volume_cb(double level);
