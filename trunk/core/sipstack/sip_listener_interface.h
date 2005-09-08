/*
  caller: SIP Stack
  recepient: Core (Statemachine)
*/

void sip_listener_incoming_call(int callId, const char *username,
								const char *domain,
								const char *callerDisplayName,
								const char *calleeUsername,
								const char *calleeDomain);

void sip_listener_cancel_call(int callId);

void sip_listener_decline_call(int callId);

void sip_listener_quit_call(int callId);

void sip_listener_throw(int callId, int code);

void sip_listener_connect_call(int callId);
