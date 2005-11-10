#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <core/events/call_status.h>

char *call_status_to_str(call_status status) {
	switch (status) {
		case CS_TRYING:
			return "TRYING";
		case CS_RINGING:
			return "RINGING";
		case CS_ACCEPTED:
			return "ACCEPTED";
		case CS_DECLINED:
			return "DECLINED";
		case CS_CANCELLED:
			return "CANCELLED";
		case CS_UNREACHABLE:
			return "UNREACHABLE";
		case CS_TERMINATED:
			return "TERMINATED";
		default:
			return "N/A";
	}
}

call_status str_to_call_status(const char *str) {
	if (strcmp(str, "TRYING") == 0) {
		return CS_TRYING;
	} else if (strcmp(str, "RINGING") == 0) {
		return CS_RINGING;
	} else if (strcmp(str, "ACCEPTED") == 0) {
		return CS_ACCEPTED;
	} else if (strcmp(str, "DECLINED") == 0) {
		return CS_DECLINED;
	} else if (strcmp(str, "CANCELLED") == 0) {
		return CS_CANCELLED;
	} else if (strcmp(str, "UNREACHABLE") == 0) {
		return CS_UNREACHABLE;
	} else if (strcmp(str, "TERMINATED") == 0) {
		return CS_TERMINATED;
	} else {
		return CS_INVALID_CALL_STATUS;
	}
}
