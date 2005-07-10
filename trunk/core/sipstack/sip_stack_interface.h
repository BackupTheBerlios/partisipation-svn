bool register(int const accountId,
          char* const callee);

int start_call();

bool cancel_call(int callId);

bool quit_call(int callId);

bool decline_call(int callId);

bool set_listener(int listener);
