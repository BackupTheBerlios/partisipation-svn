#ifndef HXMLRPC_CALLBACK_CLIENT_USED
#define HXMLRPC_CALLBACK_CLIENT_USED

#include <xmlrpc.h>

extern xmlrpc_env callbackEnv;

int cb_init();

int cb_destroy();

#endif
