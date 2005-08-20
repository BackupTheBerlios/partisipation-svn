typedef enum {
	INITIAL,
	CHECKING,
	INVITING,
	INVITED,
	CONNECTED,
	TERMINATING,
	FINAL
} sm_state;

void *sm_start(void *args);
