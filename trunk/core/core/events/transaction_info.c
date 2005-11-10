#include <stdio.h>
#include <stdlib.h>

#include <core/events/info_types.h>
#include <core/events/transaction_info.h>

int find_transaction_by_type(local_call_info * callInfo,
							 transaction_type type) {
	int i;
	for (i = 0; i < MAX_TRANSACTION_COUNT; i++) {
		if (callInfo->transactions[i].type == type) {
			return callInfo->transactions[i].id;
		}
	}
	return -1;
}

int add_transaction(local_call_info * callInfo, int transId,
					transaction_type transType) {
	int i;
	for (i = 0; i < MAX_TRANSACTION_COUNT; i++) {
		if (callInfo->transactions[i].id == -1) {
			callInfo->transactions[i].id = transId;
			callInfo->transactions[i].type = transType;
			return 1;
		}
	}
	return 0;
}

int clear_transaction(local_call_info * callInfo, int transId) {
	int i;
	for (i = 0; i < MAX_TRANSACTION_COUNT; i++) {
		if (callInfo->transactions[i].id == transId) {
			callInfo->transactions[i].id = -1;
			return 1;
		}
	}
	return 0;
}

int init_transactions(local_call_info * callInfo) {
	int i;
	callInfo->transactions =
		(transaction *) malloc(sizeof(transaction) *
							   MAX_TRANSACTION_COUNT);

	for (i = 0; i < MAX_TRANSACTION_COUNT; i++) {
		callInfo->transactions[i].id = -1;
	}
	return 1;
}

int destroy_transactions(local_call_info * callInfo) {
	if (callInfo->transactions) {
		free(callInfo->transactions);
	}
	return 1;
}
