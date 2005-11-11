#ifndef H_TRANSACTION_INFO_USED
#define H_TRANSACTION_INFO_USED

#include <core/events/info_types.h>

int find_transaction_by_type(local_call_info * callInfo,
							 transaction_type type);

int add_transaction(local_call_info * callInfo, int transId,
					transaction_type transType);

void update_transaction_and_dialog(local_call_info * callInfo, int transId,
								   transaction_type transType,
								   int dialogId);

int clear_transaction(local_call_info * callInfo, int transId);

int init_transactions(local_call_info * callInfo);

int destroy_transactions(local_call_info * callInfo);

#endif
