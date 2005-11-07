#include <stdlib.h>
#include <stdio.h>

#include <remote/server/accounts.h>
#include <core/gui_input/accounts_receiver.h>

void account_get_all(int *accountIds, int *length) {
	gi_account_get_all(accountIds, length);
}

int account_set(int const accountId, char *const attribute,
				char *const value) {
	return gi_account_set(accountId, attribute, value);
}

void account_get(int const accountId, char *const attribute, char *result) {
	gi_account_get(accountId, attribute, result);
}

int account_create() {
	return gi_account_create();
}

int account_delete(int const accountId) {
	return gi_account_delete(accountId);
}

int account_register(int const accountId) {
	return gi_account_register(accountId);
}

int account_unregister(int const accountId) {
	return gi_account_unregister(accountId);
}

int account_register_auto() {
	return gi_account_register_auto();
}

int account_save() {
	return gi_account_save();
}
