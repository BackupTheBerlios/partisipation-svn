#ifndef HREGISTRATION_REMOTE_USED
#define HREGISTRATION_REMOTE_USED

#include <xmlrpc.h>

xmlrpc_value *register_gui_RP(xmlrpc_env * const env,
							  xmlrpc_value * const param_array,
							  void *const server_context);

xmlrpc_value *unregister_gui_RP(xmlrpc_env * const env,
								xmlrpc_value * const param_array,
								void *const server_context);

#endif
