#include <xmlrpc.h>

xmlrpc_value *account_get_all_RP(xmlrpc_env * const env,
								 xmlrpc_value * const param_array,
								 void *const server_context);

xmlrpc_value *account_set_RP(xmlrpc_env * const env,
							 xmlrpc_value * const param_array,
							 void *const server_context);

xmlrpc_value *account_get_RP(xmlrpc_env * const env,
							 xmlrpc_value * const param_array,
							 void *const server_context);

xmlrpc_value *account_create_RP(xmlrpc_env * const env,
								xmlrpc_value * const param_array,
								void *const server_context);

xmlrpc_value *account_delete_RP(xmlrpc_env * const env,
								xmlrpc_value * const param_array,
								void *const server_context);

xmlrpc_value *account_register_RP(xmlrpc_env * const env,
								  xmlrpc_value * const param_array,
								  void *const server_context);

xmlrpc_value *account_unregister_RP(xmlrpc_env * const env,
									xmlrpc_value * const param_array,
									void *const server_context);

xmlrpc_value *account_save_RP(xmlrpc_env * const env,
							  xmlrpc_value * const param_array,
							  void *const server_context);
