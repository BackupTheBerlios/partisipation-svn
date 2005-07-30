#include <xmlrpc.h>

void die_if_fault_occurred(xmlrpc_env * env);

xmlrpc_env callback_init();

void callback_cleanup(xmlrpc_env env);
