#ifndef H_CALL_STATUS_USED
#define H_CALL_STATUS_USED

typedef enum {
	CS_TRYING,
	CS_RINGING,
	CS_ACCEPTED,
	CS_DECLINED,
	CS_CANCELLED,
	CS_UNREACHABLE,
	CS_TERMINATED,
	CS_INVALID_CALL_STATUS
} call_status;

const char *call_status_to_str(call_status status);

call_status str_to_call_status(const char *str);

#endif
