#include <stdlib.h>
#include <stdio.h>

#include <core/gui_input/accounts_receiver.h>
#include <accounts/account_client_interface.h>
#include <core/sip_output/registrar_manager.h>

void gi_account_get_all(int *accountIds, int *length) {
	am_account_get_all(accountIds, length);
}

int gi_account_set(int const accountId, char *const attribute,
				   char *const value) {
	return am_account_set(accountId, attribute, value);
}

void gi_account_get(int const accountId, char *const attribute,
					char *result) {
	am_account_get(accountId, attribute, result);
}

int gi_account_create() {
	return am_account_create();
}

int gi_account_delete(int const accountId) {
	return am_account_delete(accountId);
}

int gi_account_register(int const accountId) {
	return rm_register_account(accountId);
}

int gi_account_unregister(int const accountId) {
	return rm_unregister_account(accountId);
}

int gi_account_register_auto() {
	return rm_register_auto();
}

int gi_account_save() {
	return am_account_save();
}
