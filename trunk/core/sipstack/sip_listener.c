#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>

#include <../util/logging/logger.h>

#include "sip_listener_interface.h"

#define SIP_LISTENER_MSG_PREFIX "[sip listener]"

/*
  caller: SIP Stack
  recepient: Core (Statemachine)
*/

void sip_listener_incoming_call(int callId, const char *username,
								const char *domain,
								const char *callerDisplayName,
								const char *calleeUsername,
								const char *calleeDomain) {

	log_message(LOG_INFO,
				SIP_LISTENER_MSG_PREFIX
				"Received new call: [To: %s@%s, From: %s <%s@%s>, %s]",
				calleeUsername, calleeDomain, callerDisplayName, username,
				domain);
}

void sip_listener_cancel_call(int callId) {
	log_message(LOG_INFO,
				SIP_LISTENER_MSG_PREFIX "Received CANCEL for call %i",
				callId);
}

void sip_listener_decline_call(int callId) {
	log_message(LOG_INFO,
				SIP_LISTENER_MSG_PREFIX "Received DECLINE for call %i",
				callId);
}

void sip_listener_quit_call(int callId) {
	log_message(LOG_INFO,
				SIP_LISTENER_MSG_PREFIX "Received BYE for call %i",
				callId);
}

void sip_listener_throw(int callId, int code) {
	log_message(LOG_INFO,
				SIP_LISTENER_MSG_PREFIX
				"Received message %i for call %i", code, callId);
}

void sip_listener_connect_call(int callId) {
	log_message(LOG_INFO,
				SIP_LISTENER_MSG_PREFIX "Received OK for call %i", callId);
}
