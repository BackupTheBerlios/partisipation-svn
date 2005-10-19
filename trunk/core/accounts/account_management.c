#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

#include <expat.h>
#include <lib/genx/genx.h>
#include <accounts/account_client_interface.h>
#include <accounts/account_core_interface.h>
#include <accounts/list.h>

#include <util/logging/logger.h>
#include <remote/server/constants.h>

#define ACCOUNT_MANAGER_MSG_PREFIX "[account manager]"
#define BUFFSIZE 1000

char *XMLFILE = "accounts/accounts.xml";
int id = 0;
struct account *cur_acc;

/**
	This function changes the path to XML file with the account list. Default 
	path is "account.xml" in the same directory, where account_management.c 
	is located. 
*/
void am_set_xml_source(char *file) {
	XMLFILE = file;
}

/**
	This function is called by the eXpat parser, if an XML element opening tag 
  	has been discovered.
  	
  	Each attribute within the array of attributes occupies 2 consecutive places
  	like this - array[n]: <attribute name>, array[n+1]: <attribute value>
  	
  	@param void* data User data
  	@param char* name Name of the XML element
  	@param char** attr Array of attributes of the XML element
*/
void elem_start(void *data, const char *name, const char **attr) {
	log_message(LOG_INFO, ACCOUNT_MANAGER_MSG_PREFIX "elem_start - enter");
	if (strcmp(name, "account") == 0) {
		cur_acc = (struct account *) malloc(sizeof(struct account));
		int i;
		char *val;

		for (i = 0; i < 20; i += 2) {

			int len = strlen(attr[i + 1]);
			const char *s = attr[i + 1];

			val = (char *) malloc(len);
			strcpy(val, s);

			if (strcmp(attr[i], "name") == 0) {
				cur_acc->name = val;
			} else if (strcmp(attr[i], "username") == 0) {
				cur_acc->username = val;
			} else if (strcmp(attr[i], "domain") == 0) {
				cur_acc->domain = val;
			} else if (strcmp(attr[i], "authusername") == 0) {
				cur_acc->authusername = val;
			} else if (strcmp(attr[i], "password") == 0) {
				cur_acc->password = val;
			} else if (strcmp(attr[i], "displayname") == 0) {
				cur_acc->displayname = val;
			} else if (strcmp(attr[i], "outboundproxy") == 0) {
				cur_acc->outboundproxy = val;
			} else if (strcmp(attr[i], "registrar") == 0) {
				cur_acc->registrar = val;
			} else if (strcmp(attr[i], "autoregister") == 0) {
				cur_acc->autoregister = atoi(val);
				free(val);
			} else if (strcmp(attr[i], "id") == 0) {
				int cur_id = atoi(val);

				cur_acc->id = cur_id;
				// set ID counter to the highest value
				if (cur_id > id)
					id = cur_id;
				free(val);
			}
		}
	}
	log_message(LOG_INFO, ACCOUNT_MANAGER_MSG_PREFIX "elem_start - exit");
}

/**
	This function is called by the eXpat parser, if an XML element closing tag 
	has been discovered. 
    
    @param void* data User data
    @param char* name Name of the XML element
*/
void elem_end(void *data, const char *name) {
	log_message(LOG_INFO, ACCOUNT_MANAGER_MSG_PREFIX "elem_end - start");
	if (strcmp(name, "account") == 0) {
		add_node(cur_acc);
	}
	log_message(LOG_INFO, ACCOUNT_MANAGER_MSG_PREFIX "elem_end - exit");
}

/**
	Initialize list of accounts by parsing an XML file.
*/
void am_init() {
	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_management_init() - enter");
	int xmlfile = open(XMLFILE, O_RDONLY);
	XML_Parser parser = XML_ParserCreate(NULL);

	XML_SetElementHandler(parser, elem_start, elem_end);
	for (;;) {
		int bytes_read;
		void *buf = XML_GetBuffer(parser, BUFFSIZE);

		if (buf == NULL) {
			// if memory could not be allocated
			log_message(LOG_INFO,
						ACCOUNT_MANAGER_MSG_PREFIX
						"Buffer allocation failed");

		}

		bytes_read = read(xmlfile, buf, BUFFSIZE);

		if (bytes_read < 0) {
			// if file could not be read
			log_message(LOG_INFO,
						ACCOUNT_MANAGER_MSG_PREFIX
						"No file found or file reading error");

		}

		if (!XML_ParseBuffer(parser, bytes_read, bytes_read == 0)) {
			// if XML document is not well-formed
			log_message(LOG_INFO,
						ACCOUNT_MANAGER_MSG_PREFIX
						"XML parse error in line %d: %s");

			XML_GetCurrentLineNumber(parser),
				XML_ErrorString(XML_GetErrorCode(parser));
			break;
		}
		// if end of file reached
		if (bytes_read == 0)
			break;
	}
	XML_ParserFree(parser);
	close(xmlfile);

	print_list();

	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_management_init() - exit");
}

/**
	Save current accounts to an XML file.
*/
void account_list_save() {
	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX "accounts_list_save - start");

	// open file in write-mode
	FILE *xmlfile = fopen(XMLFILE, "w");

	if (xmlfile) {

		struct node *tmp = head;

		genxWriter w = genxNew(NULL, NULL, NULL);

		genxStartDocFile(w, xmlfile);
		genxStartElementLiteral(w, NULL, "list");

		// continue until the end of the list
		while (tmp) {

			// get current account
			struct account *acc = tmp->acc;

			genxStartElementLiteral(w, NULL, "account");

			char *str_id = (char *) malloc(20);
			snprintf(str_id, 10, "%d", acc->id);
			genxAddAttributeLiteral(w, NULL, "id", str_id);
			free(str_id);

			genxAddAttributeLiteral(w, NULL, "name", acc->name);
			genxAddAttributeLiteral(w, NULL, "username", acc->username);
			genxAddAttributeLiteral(w, NULL, "domain", acc->domain);
			genxAddAttributeLiteral(w, NULL, "authusername",
									acc->authusername);
			genxAddAttributeLiteral(w, NULL, "password", acc->password);
			genxAddAttributeLiteral(w, NULL, "displayname",
									acc->displayname);
			genxAddAttributeLiteral(w, NULL, "outboundproxy",
									acc->outboundproxy);
			genxAddAttributeLiteral(w, NULL, "registrar", acc->registrar);

			char *str_autoreg = (char *) malloc(20);
			snprintf(str_autoreg, 10, "%d", acc->autoregister);
			genxAddAttributeLiteral(w, NULL, "autoregister", str_autoreg);
			free(str_autoreg);

			genxEndElement(w);

			tmp = tmp->next;

		}

		genxEndElement(w);
		genxEndDocument(w);

	} else {
		log_message(LOG_INFO,
					ACCOUNT_MANAGER_MSG_PREFIX
					"I/O error for file %s", XMLFILE);
		// printf("I/O error for file %s\n", XMLFILE);                                              
	}
	// close file
	fclose(xmlfile);
	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX "accounts_list_save - exit");

}

/**
	This function delivers the IDs of all known accounts.
	
	@param int* accountIds Pointer to array to be filled with account IDs
	@param int* length Pointer to an integer to store length of this array
*/
void am_account_get_all(int *accountIds, int *length) {
	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_get_all() - enter");

	*length = get_length();

	if (*length < MAX_ACCOUNTID_AMOUNT) {
		int i = 0;
		struct node *tmp = head;

		while (tmp) {
			accountIds[i] = tmp->acc->id;
			tmp = tmp->next;
			i++;
		}
	} else {
		*length = 0;
		log_message(LOG_INFO,
					ACCOUNT_MANAGER_MSG_PREFIX
					"Error: More accounts than memory reserved");
	}
	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_get_all() - exit");

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

	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_set() - enter");

	struct account *acc = get_node(accountId)->acc;
	char *new_val = (char *) malloc(strlen(value));

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

	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_set() - exit");

	return 1;
}

/**
	This function delivers certain attribute value for the specified account.
	
	Note that only as much characters are read as defined by MAX_VALUE_LENGTH
	from <constants.h>. Anything else is cut away, but this length ought be
	enough for any convenient value. 
	
	@param int accountId ID of the account whose attribute is to be read
	@param char attribute Name of the attribute to be read
	@param char* result Char-pointer to store the value of the attribute in
*/
void am_account_get(int const accountId, char *const attribute,
					char *result) {
	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_get() - enter");

	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"id: %d, att: %s", accountId, attribute);
	print_list();

	struct account *acc = get_node(accountId)->acc;

	if (strcmp(attribute, "id") == 0) {
		snprintf(result, 10, "%d", acc->id);
	} else if (strcmp(attribute, "name") == 0) {
		strncpy(result, acc->name, MAX_VALUE_LENGTH);
	} else if (strcmp(attribute, "username") == 0) {
		strncpy(result, acc->username, MAX_VALUE_LENGTH);
	} else if (strcmp(attribute, "domain") == 0) {
		strncpy(result, acc->domain, MAX_VALUE_LENGTH);
	} else if (strcmp(attribute, "authusername") == 0) {
		strncpy(result, acc->authusername, MAX_VALUE_LENGTH);
	} else if (strcmp(attribute, "password") == 0) {
		strncpy(result, acc->password, MAX_VALUE_LENGTH);
	} else if (strcmp(attribute, "displayname") == 0) {
		strncpy(result, acc->displayname, MAX_VALUE_LENGTH);
	} else if (strcmp(attribute, "outboundproxy") == 0) {
		strncpy(result, acc->outboundproxy, MAX_VALUE_LENGTH);
	} else if (strcmp(attribute, "registrar") == 0) {
		strncpy(result, acc->registrar, MAX_VALUE_LENGTH);
	} else if (strcmp(attribute, "autoregister") == 0) {
		snprintf(result, 10, "%d", acc->autoregister);
	}

	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_get() - exit");
}

/**
	This function creates new account.
	
	@return int ID of the new account
*/
int am_account_create() {
	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_create() - enter");

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

	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_create() - exit");

	return id;
}

/**
	This function deletes account with the specified ID.
	
	@param int accountId ID of the account to be deleted
	@return int Always 1
*/
int am_account_delete(int const accountId) {
	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_delete() - enter");

	del_node(accountId);

	account_list_save();
	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_delete() - exit");

	return 1;
}

/**
 * This function writes data for all accounts from memory to a file on the disk.
 */
int am_account_save() {
	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_save() - enter");

	account_list_save();
	log_message(LOG_INFO,
				ACCOUNT_MANAGER_MSG_PREFIX
				"account_management.c - account_save() - exit");
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

	if (*length < MAX_ACCOUNTID_AMOUNT) {
		int i = 0;
		struct node *tmp = head;

		while (tmp) {
			accounts[i] = tmp->acc;
			tmp = tmp->next;
			i++;
		}
	} else {
		*length = 0;
		log_message(LOG_INFO,
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
	return n->acc;
}
