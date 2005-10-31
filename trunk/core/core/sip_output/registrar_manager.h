#ifndef HREGISTRAR_MANAGER_USED
#define HREGISTRAR_MANAGER_USED

#include <sipstack/sip_stack_interface.h>

int rm_init();

int rm_register_account(int accountId);

int rm_unregister_account(int accountId);

int rm_register_auto();

int rm_receive_register_event(sipstack_event * event);

int rm_destroy();

#endif
