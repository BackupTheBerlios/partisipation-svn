#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

#include <lib/genx/genx.h>
#include <accounts/account_client_interface.h>
#include <accounts/account_core_interface.h>
#include <util/list/list.h>
#include <util/config/xml/config_writer.h>
#include <util/logging/logger.h>

#define ACCOUNT_MANAGER_MSG_PREFIX "[account manager] "

int id = 0;
struct account *cur_acc;

// forward declaration to prevent warning:
int am_get_max_id();

/**
	Initialize list of accounts.
*/
void am_init() {
	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management_init() - enter");

	id = am_get_max_id();

	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management_init() - exit");
}

/**
	This function delivers the IDs of all known accounts.
	
	@param int* accountIds Pointer to array to be filled with account IDs
	@param int* length Pointer to an integer to store length of this array
*/
void am_account_get_all(int *accountIds, int *length) {
	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX "am_get_all() - enter");

	*length = get_length();

	if (*length < config.accounts.accountManagement.maxAccountIdAmount) {
		int i = 0;
		struct node *tmp = head;

		while (tmp) {
			accountIds[i] = tmp->acc->id;
			tmp = tmp->next;
			i++;
		}
	} else {
		*length = 0;
		log_message(LOG_ERROR,
					ACCOUNT_MANAGER_MSG_PREFIX
					"Error: More accounts than memory reserved");
	}
	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX "am_get_all() - exit");

}

/**
	This function sets certain account attribute to the specified value.
	
	@param int accountId ID of the account whose attribute is to be set
	@param char* attribute Name of the attribute to be set
	@param char* value Value of the attribute to be set
	@return int 1 if OK, 0 if error
*/
int am_account_set(int const accountId, char *const attribute,
				   char *const value) {

	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX "am_account_set() - enter");

	struct node *n;
	struct account *acc;

	n = get_node(accountId);
	if (!n) {
		// accountId is not valid
		return 0;
	}
	acc = n->acc;

	char *new_val = (char *) malloc(strlen(value) + 1);

	strcpy(new_val, value);

	if (strcmp(attribute, "id") == 0) {
		acc->id = atoi(value);
		free(new_val);
	} else if (strcmp(attribute, "name") == 0) {
		free(acc->name);
		acc->name = new_val;
	} else if (strcmp(attribute, "username") == 0) {
		free(acc->username);
		acc->username = new_val;
	} else if (strcmp(attribute, "domain") == 0) {
		free(acc->domain);
		acc->domain = new_val;
	} else if (strcmp(attribute, "authusername") == 0) {
		free(acc->authusername);
		acc->authusername = new_val;
	} else if (strcmp(attribute, "password") == 0) {
		free(acc->password);
		acc->password = new_val;
	} else if (strcmp(attribute, "displayname") == 0) {
		free(acc->displayname);
		acc->displayname = new_val;
	} else if (strcmp(attribute, "outboundproxy") == 0) {
		free(acc->outboundproxy);
		acc->outboundproxy = new_val;
	} else if (strcmp(attribute, "registrar") == 0) {
		free(acc->registrar);
		acc->registrar = new_val;
	} else if (strcmp(attribute, "autoregister") == 0) {
		acc->autoregister = atoi(value);
		free(new_val);
	} else {
		free(new_val);
		return 0;
	}

	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX "am_account_set() - exit");

	return 1;
}

/**
	This function delivers certain attribute value for the specified account.
	
	Note that only as much characters are read as defined by config.accounts.accountManagement.maxValueLength
	from <constants.h>. Anything else is cut away, but this length ought be
	enough for any convenient value. 
	
	@param int accountId ID of the account whose attribute is to be read
	@param char attribute Name of the attribute to be read
	@param char* result Char-pointer to store the value of the attribute in
*/
void am_account_get(int const accountId, char *const attribute,
					char *result) {
	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX "am_account_get() - enter");

	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX
				"id: %d, att: %s", accountId, attribute);

	struct node *n;
	struct account *acc;

	n = get_node(accountId);
	if (!n) {
		// accountId is not valid
		return;
	}
	acc = n->acc;

	if (strcmp(attribute, "id") == 0) {
		snprintf(result, 10, "%d", acc->id);
	} else if (strcmp(attribute, "name") == 0) {
		strncpy(result, acc->name,
				config.accounts.accountManagement.maxValueLength);
	} else if (strcmp(attribute, "username") == 0) {
		strncpy(result, acc->username,
				config.accounts.accountManagement.maxValueLength);
	} else if (strcmp(attribute, "domain") == 0) {
		strncpy(result, acc->domain,
				config.accounts.accountManagement.maxValueLength);
	} else if (strcmp(attribute, "authusername") == 0) {
		strncpy(result, acc->authusername,
				config.accounts.accountManagement.maxValueLength);
	} else if (strcmp(attribute, "password") == 0) {
		strncpy(result, acc->password,
				config.accounts.accountManagement.maxValueLength);
	} else if (strcmp(attribute, "displayname") == 0) {
		strncpy(result, acc->displayname,
				config.accounts.accountManagement.maxValueLength);
	} else if (strcmp(attribute, "outboundproxy") == 0) {
		strncpy(result, acc->outboundproxy,
				config.accounts.accountManagement.maxValueLength);
	} else if (strcmp(attribute, "registrar") == 0) {
		strncpy(result, acc->registrar,
				config.accounts.accountManagement.maxValueLength);
	} else if (strcmp(attribute, "autoregister") == 0) {
		snprintf(result, 10, "%d", acc->autoregister);
	}

	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX "am_account_get() - exit");
}

/**
	This function creates new account.
	
	@return int ID of the new account
*/
int am_account_create() {
	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX "am_account_create() - enter");

	struct account *a = (struct account *) malloc(sizeof(struct account));

	id++;

	a->id = id;
	a->autoregister = 0;
	a->name = NULL;
	a->username = NULL;
	a->domain = NULL;
	a->authusername = NULL;
	a->password = NULL;
	a->displayname = NULL;
	a->outboundproxy = NULL;
	a->registrar = NULL;

	add_node(a);

	cw_save_config();

	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX "am_account_create() - exit");

	return id;
}

/**
	This function deletes account with the specified ID.
	
	@param int accountId ID of the account to be deleted
	@return int Always 1
*/
int am_account_delete(int const accountId) {
	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX "am_account_delete() - enter");

	del_node(accountId);

	cw_save_config();

	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX "account_delete() - exit");

	return 1;
}

/**
 * This function writes data for all accounts from memory to a file on the disk.
 */
int am_account_save() {
	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX "am_account_save() - enter");

	cw_save_config();

	log_message(LOG_DEBUG,
				ACCOUNT_MANAGER_MSG_PREFIX "am_account_save() - exit");
	return 1;
}

/**
 * This function filles a given array with pointers to all known accounts. The caller is
 * responsible to allocate and free memory for this array.
 *  
 * @param struct account* accounts[]  array, which will be filled with pointers to all accounts
 * @param int* length tells, how big this array is
 */
void am_get_all_accounts(struct account *accounts[], int *length) {

	*length = get_length();

	if (*length < config.accounts.accountManagement.maxAccountIdAmount) {
		int i = 0;
		struct node *tmp = head;

		while (tmp) {
			accounts[i] = tmp->acc;
			tmp = tmp->next;
			i++;
		}
	} else {
		*length = 0;
		log_message(LOG_ERROR,
					ACCOUNT_MANAGER_MSG_PREFIX
					"Error: More accounts than memory reserved");

	}
}

/**
 * This function returns a pointer to the account with a special ID.
 * 
 * @param int accountId ID of the account
 * @return struct account* pointer to the account with this ID
 */
struct account *am_get_account(int const accountId) {
	struct node *n = get_node(accountId);
	if (!n) {
		return NULL;
	}
	return n->acc;
}

/**
 * This function goes through the list of accounts to find maximal ID.
 * 
 * @return maximal ID value
 */
int am_get_max_id() {
	int max = 0;

	struct node *tmp = head;

	while (tmp) {
		struct account *acc = tmp->acc;

		if (acc->id > max)
			max = acc->id;

		tmp = tmp->next;
	}
	return max;
}

int am_get_account_by_callee_uri(char *calleeSipUri) {
	struct account *accounts[config.accounts.accountManagement.
							 maxAccountIdAmount];
	int len;					// length of account data array
	int i;
	char *uri;
	char *username;
	char *domain;
	char *protocol;

	uri = (char *) malloc(strlen(calleeSipUri) * sizeof(char) + 1);
	strcpy(uri, calleeSipUri);

	protocol = strtok(uri, ":");
	if (!protocol || strcmp("sip", protocol) != 0) {
		LOG_DEBUG(ACCOUNT_MANAGER_MSG_PREFIX "get_account_by_callee_uri: "
				  "wrong protocol in URI <%s>, expecting \"sip:\"",
				  calleeSipUri);
		free(uri);
		return -1;
	}

	username = strtok(NULL, "@");
	if (!username) {
		LOG_DEBUG(ACCOUNT_MANAGER_MSG_PREFIX "get_account_by_callee_uri: "
				  "@ is missing in URI <%s>", calleeSipUri);
		free(uri);
		return -1;
	}

	domain = strtok(NULL, "");
	if (!domain) {
		LOG_DEBUG(ACCOUNT_MANAGER_MSG_PREFIX "get_account_by_callee_uri: "
				  "domain is empty in URI <%s>", calleeSipUri);
		free(uri);
		return -1;
	}
	// retrieve all account data:
	am_get_all_accounts(accounts, &len);

	// check for accounts with matching user data
	for (i = 0; i < len; i++) {
		if (strcmp(accounts[i]->username, username) == 0
			&& strcmp(accounts[i]->domain, domain) == 0) {
			free(uri);
			return accounts[i]->id;
		}
	}

	LOG_DEBUG(ACCOUNT_MANAGER_MSG_PREFIX "get_account_by_callee_uri: "
			  "no account found, URI <%s>, username <%s>, domain <%s>",
			  calleeSipUri, username, domain);
	free(uri);
	return -1;
}

int am_is_account_valid(int accountId) {
	struct account *acc;
	acc = am_get_account(accountId);
	if (!acc) {
		return 0;
	}
	if (!acc->domain || !acc->username) {
		// main account data missing
		return 0;
	}

	if (strcmp(acc->domain, "") == 0 || strcmp(acc->username, "") == 0) {
		// main account data empty
		return 0;
	}

	return 1;
}

int am_build_from_and_to(int accountId, char *callee, char **from,
						 char **to) {
	struct account *acc;
	char *at;
	acc = am_get_account(accountId);
	if (!acc) {
		return 0;
	}

	if (!callee || strlen(callee) == 0) {
		return 0;
	}

	*from = (char *)
		malloc((strlen(acc->username) + strlen(acc->domain) +
				strlen("sip:") + strlen("@")) * sizeof(char) + 1);

	sprintf(*from, "sip:%s@%s", acc->username, acc->domain);

	at = strstr(callee, "@");
	if (!at) {
		*to = (char *)
			malloc((strlen(callee) + strlen(acc->domain) + strlen("sip:") +
					strlen("@")) * sizeof(char) + 1);
		sprintf(*to, "sip:%s@%s", callee, acc->domain);
	} else {
		*to =
			(char *) malloc((strlen(callee) + strlen("sip:")) *
							sizeof(char) + 1);
		sprintf(*to, "sip:%s", callee);
	}

	return 1;
}
