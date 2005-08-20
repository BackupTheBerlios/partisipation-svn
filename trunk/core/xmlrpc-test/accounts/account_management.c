#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <expat.h>
#include <accounts/account_management.h>
#include <constants.h>
#include <accounts/list.h>

#define XMLFILE "accounts/accounts.xml"
#define BUFFSIZE 1000

int id = 1;
char *cur_elem;
struct account *cur_acc;

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
	cur_elem = (char *) realloc(cur_elem, strlen(name));
	if (cur_elem) {
		strcpy(cur_elem, name);
		if (strcmp(cur_elem, "account") == 0) {
			cur_acc = (struct account *) malloc(sizeof(struct account));
			int i;
			char *val;

			for (i = 0; i < 20; i += 2) {

				int len = strlen(attr[i + 1]);
				const char *s = attr[i + 1];

				int empty = 1;
				int j;

				for (j = 0; j < len; j++) {
					if (isgraph(s[j])) {
						empty = 0;
						break;
					}
				}

				if (empty)
					val = NULL;
				else {
					val = (char *) malloc(len);
					strcpy(val, s);
				}

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
	} else {
		printf("Error reallocating memory.\n");
	}
}

/**
	This function is called by the eXpat parser, if an XML element closing tag 
	has been discovered. 
    
    @param void* data User data
    @param char* name Name of the XML element
*/
void elem_end(void *data, const char *name) {
	// add account to the list
	if (strcmp(cur_elem, "account") == 0) {
		add_node(cur_acc);
	}
	free(cur_elem);
}

/**
	Initialize list of accounts by parsing an XML file.
*/
void account_management_init() {

	printf("account_management.c - account_management_init() - enter\n");

	int xmlfile = open(XMLFILE, O_RDONLY);
	XML_Parser parser = XML_ParserCreate(NULL);

	XML_SetElementHandler(parser, elem_start, elem_end);
	for (;;) {
		int bytes_read;
		void *buf = XML_GetBuffer(parser, BUFFSIZE);

		if (buf == NULL) {
			// if memory could not be allocated
			printf("Buffer allocation failed.\n");
		}
		bytes_read = read(xmlfile, buf, BUFFSIZE);

		if (bytes_read < 0) {
			// if file could not be read
			printf("File reading error.\n");
		}

		if (!XML_ParseBuffer(parser, bytes_read, bytes_read == 0)) {
			// if XML document is not well-formed
			printf("XML parse error in line %d: %s\n",
				   XML_GetCurrentLineNumber(parser),
				   XML_ErrorString(XML_GetErrorCode(parser)));
			break;
		}
		// if end of file reached
		if (bytes_read == 0)
			break;
	}
	XML_ParserFree(parser);
	close(xmlfile);

	printf("account_management.c - account_management_init() - exit\n");
}

/**
	Save current accounts to an XML file.
*/
void account_list_save() {
	// open file in write-mode
	FILE *xmlfile = fopen(XMLFILE, "w");

	if (xmlfile) {
		// write the XML declaration as wide-character string
		fprintf(xmlfile,
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
		// open document root element
		fprintf(xmlfile, "<list>\n");

		struct node *tmp = head;

		// continue until the end of the list
		while (tmp) {
			// get current account
			struct account *acc = tmp->acc;

			// save account data
			fprintf(xmlfile, "\t<account\n");
			fprintf(xmlfile, "\t\tid=\"%d\"\n", acc->id);
			fprintf(xmlfile, "\t\tname=\"%s\"\n", acc->name);
			fprintf(xmlfile, "\t\tusername=\"%s\"\n", acc->username);
			fprintf(xmlfile, "\t\tdomain=\"%s\"\n", acc->domain);
			fprintf(xmlfile, "\t\tauthusername=\"%s\"\n",
					acc->authusername);
			fprintf(xmlfile, "\t\tpassword=\"%s\"\n", acc->password);
			fprintf(xmlfile, "\t\tdisplayname=\"%s\"\n", acc->displayname);
			fprintf(xmlfile, "\t\toutboundproxy=\"%s\"\n",
					acc->outboundproxy);
			fprintf(xmlfile, "\t\tregistrar=\"%s\"\n", acc->registrar);
			fprintf(xmlfile, "\t\tautoregister=\"%d\"\n",
					acc->autoregister);
			fprintf(xmlfile, "\t/>\n");
			// go on with next account
			tmp = tmp->next;
		}
		// close document root element
		fprintf(xmlfile, "</list>\n");
	} else {
		printf("I/O error for file %s\n", XMLFILE);
	}
	// close file
	fclose(xmlfile);
}

/**
	This function delivers the IDs of all known accounts.
	
	@param int* accountIds Array of account IDs to be filled
	@param int* length ???
*/
void account_get_all(int *accountIds, int *length) {

	printf("account_management.c - account_get_all() - enter\n");

	if (*length < MAX_ACCOUNTID_AMOUNT) {
		int i = 0;
		struct node *tmp = head;

		while (tmp) {
			accountIds[i] = tmp->acc->id;
			tmp = tmp->next;
			i++;
		}
	} else {
		printf("Error: ???");
	}
	printf("account_management.c - account_get_all() - exit\n");
}

/**
	This function sets certain account attribute to the specified value.
	
	@param int accountId ID of the account whose attribute is to be set
	@param char* attribute Name of the attribute to be set
	@param char* value Value of the attribute to be set
	@return int ???
*/
int
account_set(int const accountId, char *const attribute,
			char *const value) {

	printf("account_management.c - account_set() - enter\n");

	struct account *acc = get_node(accountId)->acc;
	char *new_val = (char *) malloc(strlen(value));

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

	printf("account_management.c - account_set() - exit\n");

	return 1;
}

/**
	This function delivers certain attribute value for the specified account.
	
	@param int accountId ID of the account whose attribute is to be read
	@param char attribute Name of the attribute to be read
	@return char* Value of the specified attribute
*/
char *account_get(int const accountId, char *const attribute) {

	printf("account_management.c - account_get() - enter\n");

	struct account *acc = get_node(accountId)->acc;

	if (strcmp(attribute, "id") == 0) {
		char buf[10];

		snprintf(buf, sizeof(buf), "%d", acc->id);
		return buf;
	} else if (strcmp(attribute, "name") == 0) {
		return acc->name;
	} else if (strcmp(attribute, "username") == 0) {
		return acc->username;
	} else if (strcmp(attribute, "domain") == 0) {
		return acc->domain;
	} else if (strcmp(attribute, "authusername") == 0) {
		return acc->authusername;
	} else if (strcmp(attribute, "password") == 0) {
		return acc->password;
	} else if (strcmp(attribute, "displayname") == 0) {
		return acc->displayname;
	} else if (strcmp(attribute, "outboundproxy") == 0) {
		return acc->outboundproxy;
	} else if (strcmp(attribute, "registrar") == 0) {
		return acc->registrar;
	} else if (strcmp(attribute, "autoregister") == 0) {
		char buf[10];

		snprintf(buf, sizeof(buf), "%d", acc->autoregister);
		return buf;
	} else {
		return NULL;
	}
}

/**
	This function creates new account.
	
	@return int ID of the new account
*/
int account_create() {

	struct account *a = (struct account *) malloc(sizeof(struct account));

	id++;

	a->id = id;
	a->autoregister = 0;

	add_node(a);

	return id;
}

/**
	This function deletes account with the specified ID.
	
	@param int accountId ID of the account to be deleted
	@return int ???
*/
int account_delete(int const accountId) {

	del_node(accountId);

	return 1;
}

int account_register(int const accountId) {

	// do something
	printf("accountId: %d\n", accountId);

	return 1;
}

int account_unregister(int const accountId) {

	// do something
	printf("accountId: %d\n", accountId);

	return 1;
}
