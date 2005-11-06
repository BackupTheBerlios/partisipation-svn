#ifndef HACCOUNT_CORE_INTERFACE_USED
#define HACCOUNT_CORE_INTERFACE_USED

#include <util/config/globals.h>

void am_init();

void am_get_all_accounts(struct account *accounts[], int *length);

struct account *am_get_account(int const accountId);

int get_account_by_callee_uri(char *calleeSipUri);

#endif
