int sipstack_init();

int sipstack_quit();

int sipstack_send_register(char *const identity, char *const registrar,
                           int expire);

int sipstack_update_register(int id, int expire);

int sipstack_unregister(int id);

int sipstack_start_call(int callId);

int sipstack_cancel_call(int callId);

int sipstack_quit_call(int callId);

int sipstack_decline_call(int callId);

int sipstack_set_listener(int listener);
