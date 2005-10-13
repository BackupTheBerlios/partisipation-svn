#ifndef HACCOUNT_CORE_INTERFACE_USED
#define HACCOUNT_CORE_INTERFACE_USED

#include "list.h"

void am_set_xml_source(char *file);

void am_init();

void am_get_all_accounts(struct account *accounts[], int *length);

struct account *am_get_account(int const accountId);

#endif
