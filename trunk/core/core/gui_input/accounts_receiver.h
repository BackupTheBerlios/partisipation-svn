#ifndef HACCOUNTS_RECEIVER_USED
#define HACCOUNTS_RECEIVER_USED

void gi_account_get_all(int *accountIds, int *length);

int gi_account_set(int const accountId, char *const attribute,
				   char *const value);

void gi_account_get(int const accountId, char *const attribute,
					char *result);

int gi_account_create();

int gi_account_delete(int const accountId);

int gi_account_register(int const accountId);

int gi_account_unregister(int const accountId);

int gi_account_save();

#endif
