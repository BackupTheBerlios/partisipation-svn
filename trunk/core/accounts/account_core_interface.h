#ifndef HACCOUNT_CORE_INTERFACE_USED
#define HACCOUNT_CORE_INTERFACE_USED

#include <util/config/globals.h>

void am_init();

void am_get_all_accounts(struct account *accounts[], int *length);

struct account *am_get_account(int const accountId);

int am_get_account_by_callee_uri(char *calleeSipUri);

int am_is_account_valid(int accountId);

int am_build_from_and_to(int accountId, char *callee, char **from,
						 char **to);

#endif
