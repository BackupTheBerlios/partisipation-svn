#include <stdlib.h>
#include <stdio.h>

#include <xmlrpc.h>
#include <remote/server/xmlrpc/accounts_remote.h>
#include <remote/server/accounts.h>
#include <util/config/globals.h>
#include <util/logging/logger.h>

#define ACCOUNTS_REMOTE_MSG_PREFIX "[accounts remote] "

xmlrpc_value *account_get_all_RP(xmlrpc_env * const env,
								 xmlrpc_value * const param_array,
								 void *const server_context) {

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "entering accountGetAll...");

	int *ids;
	int len;
	int i;

	ids =
		(int *) malloc(config.accounts.accountManagement.
					   maxAccountIdAmount * sizeof(int));
	account_get_all(ids, &len);

	xmlrpc_value *result = xmlrpc_array_new(env);

	for (i = 0; i < len; i++) {
		xmlrpc_value *item = xmlrpc_build_value(env, "i", ids[i]);

		xmlrpc_array_append_item(env, result, item);
	}
	free(ids);

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "leaving accountGetAll");
	return result;
}

xmlrpc_value *account_set_RP(xmlrpc_env * const env,
							 xmlrpc_value * const param_array,
							 void *const server_context) {

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "entering accountSet...");

	xmlrpc_int32 accountId;
	char *attribute;
	char *value;
	int result;

	xmlrpc_decompose_value(env, param_array, "(iss)", &accountId,
						   &attribute, &value);
	if (env->fault_occurred) {
		return NULL;
	}

	result = account_set(accountId, attribute, value);

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "leaving accountSet");
	return xmlrpc_build_value(env, "b", result);
}

xmlrpc_value *account_get_RP(xmlrpc_env * const env,
							 xmlrpc_value * const param_array,
							 void *const server_context) {

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "entering accountGet...");

	xmlrpc_int32 accountId;
	char *attribute;

	char *result =
		(char *) malloc(config.accounts.accountManagement.maxValueLength *
						sizeof(char));

	xmlrpc_decompose_value(env, param_array, "(is)", &accountId,
						   &attribute);
	if (env->fault_occurred) {
		return NULL;
	}

	account_get(accountId, attribute, result);

	xmlrpc_value *xmlres = xmlrpc_build_value(env, "s", result);

	free(result);

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "leaving accountGet");
	return xmlres;

}

xmlrpc_value *account_create_RP(xmlrpc_env * const env,
								xmlrpc_value * const param_array,
								void *const server_context) {

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "entering accountCreate...");

	int result;

	result = account_create();

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "leaving accountCreate");
	return xmlrpc_build_value(env, "i", result);
}

xmlrpc_value *account_delete_RP(xmlrpc_env * const env,
								xmlrpc_value * const param_array,
								void *const server_context) {

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "entering accountDelete...");

	xmlrpc_int32 accountId;
	int result;

	xmlrpc_decompose_value(env, param_array, "(i)", &accountId);
	if (env->fault_occurred) {
		return NULL;
	}

	result = account_delete(accountId);

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "leaving accountDelete");
	return xmlrpc_build_value(env, "b", result);
}

xmlrpc_value *account_register_RP(xmlrpc_env * const env,
								  xmlrpc_value * const param_array,
								  void *const server_context) {

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "entering register...");

	xmlrpc_int32 accountId;
	int result;

	xmlrpc_decompose_value(env, param_array, "(i)", &accountId);
	if (env->fault_occurred) {
		return NULL;
	}

	result = account_register(accountId);

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "leaving register");
	return xmlrpc_build_value(env, "b", result);
}

xmlrpc_value *account_unregister_RP(xmlrpc_env * const env,
									xmlrpc_value * const param_array,
									void *const server_context) {

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "entering unregister...");

	xmlrpc_int32 accountId;
	int result;

	xmlrpc_decompose_value(env, param_array, "(i)", &accountId);
	if (env->fault_occurred) {
		return NULL;
	}

	result = account_unregister(accountId);

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "leaving unregister");
	return xmlrpc_build_value(env, "b", result);
}

xmlrpc_value *account_register_auto_RP(xmlrpc_env * const env,
									   xmlrpc_value * const param_array,
									   void *const server_context) {

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "entering registerAuto...");

	int result;

	result = account_register_auto();

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "leaving registerAuto");
	return xmlrpc_build_value(env, "b", result);
}

xmlrpc_value *account_save_RP(xmlrpc_env * const env,
							  xmlrpc_value * const param_array,
							  void *const server_context) {

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "entering accountSave...");

	int result;

	result = account_save();

	LOG_DEBUG(ACCOUNTS_REMOTE_MSG_PREFIX "leaving accountSave");
	return xmlrpc_build_value(env, "i", result);
}
