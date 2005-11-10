#ifndef H_INFO_TYPES_USED
#define H_INFO_TYPES_USED

#define MAX_TRANSACTION_COUNT 10

typedef enum {
	INVITE_TRANSACTION,
	CANCEL_TRANSACTION
} transaction_type;

typedef struct {
	int id;
	transaction_type type;
} transaction;

typedef struct {
	int callId;
	int accountId;
	char *callee;
	char *caller;
	char *from;
	char *to;
	int sipCallId;
	int dialogId;
	transaction *transactions;
	int queueId;
} local_call_info;

#endif
