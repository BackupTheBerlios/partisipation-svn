typedef struct {
    int status_code;
    char *message;
    int dialogId;
    int transactionId;
} sipstack_event;

int sipstack_init();

int sipstack_quit();

sipstack_event sipstack_receive_response(int timeout);

int sipstack_send_register(char *const identity, char *const registrar,
                           int expire);

int sipstack_send_update_register(int id, int expire);

int sipstack_send_unregister(int id);

int sipstack_send_invite(char *to, char *from, char *subject);

int sipstack_send_reinvite(int dialogId);

int sipstack_send_cancel(int callId, int dialogId);

int sipstack_send_bye(int callId, int dialogId);

int sipstack_send_decline(int callId, int dialogId);

int sipstack_send_ok(int transactionId, int dialogId);

int sipstack_send_acknowledgement(int dialogId);

int sipstack_send_status_code(int transactionId, int status_code);

int sipstack_set_listener(int listener);
