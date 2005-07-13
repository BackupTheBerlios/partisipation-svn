int send_register(int const accountId, char* const callee);

int start_call();

int cancel_call(int callId);

int quit_call(int callId);

int decline_call(int callId);

int set_listener(int listener);
