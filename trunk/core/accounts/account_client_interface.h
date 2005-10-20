#ifndef HACCOUNT_CLIENT_INTERFACE_USED
#define HACCOUNT_CLIENT_INTERFACE_USED

void am_account_get_all(int *accountIds, int *length);

int am_account_set(int const accountId, char *const attribute,
				   char *const value);

void am_account_get(int const accountId, char *const attribute,
					char *result);

int am_account_create();

int am_account_delete(int const accountId);

int am_account_save();

#endif
