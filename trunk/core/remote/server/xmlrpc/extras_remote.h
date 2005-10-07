#ifndef HEXTRAS_REMOTE_USED
#define HEXTRAS_REMOTE_USED

#include <xmlrpc.h>

xmlrpc_value *send_dtmf_RP(xmlrpc_env * const env,
						   xmlrpc_value * const param_array,
						   void *const server_context);

#endif
