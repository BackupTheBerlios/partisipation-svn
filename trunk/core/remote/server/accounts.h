#ifndef HACCOUNTS_USED
#define HACCOUNTS_USED

void account_get_all(int *accountIds, int *length);

int account_set(int const accountId, char *const attribute,
				char *const value);

void account_get(int const accountId, char *const attribute, char *result);

int account_create();

int account_delete(int const accountId);

int account_register(int const accountId);

int account_unregister(int const accountId);

int account_register_auto();

int account_save();

#endif
