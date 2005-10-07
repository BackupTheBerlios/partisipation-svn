void am_account_get_all(int *accountIds, int *length);

int am_account_set(int const accountId, char *const attribute,
				   char *const value);

void am_account_get(int const accountId, char *const attribute,
					char *result);

int am_account_create();

int am_account_delete(int const accountId);

int am_account_register(int const accountId);

int am_account_unregister(int const accountId);

int am_account_save();
