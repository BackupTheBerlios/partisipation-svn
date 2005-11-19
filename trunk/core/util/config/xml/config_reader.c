#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <expat.h>

#include <util/config/xml/config_reader.h>
#include <util/config/globals.h>
#include <util/list/list.h>

configcfg config;

const int BUFFSIZE = 1024;

int withinConfig;
int withinAccounts;
int withinCore;
int withinRemote;
int withinSipstack;
int withinUtil;
int withinLogging;
int withinSimpleLogger;
int withinConsole;
int withinFile;
int withinThreadManagement;
int withinThreads;
int withinServer;
int withinCallback;
int withinXmlrpcServer;
int withinXmlrpcClient;
int withinEvents;
int withinDispatcher;
int withinList;
int withinExosipAdapter;
int withinAccountManagement;
int withinSipOutput;
int withinRegistrarManager;

int parse_bool(const char *input) {
	if (strcmp(input, "true") == 0) {
		return 1;
	}

	if (strcmp(input, "false") == 0) {
		return 0;
	}
	// ERROR
	return -1;
}

int parse_int(const char *input) {
	return atoi(input);
}

void parse_string(const char *input, char **target) {
	int len;
	if (!input) {
		*target = (char *) malloc(1);
		strcpy(*target, "");
		return;
	}
	len = strlen(input);
	*target = (char *) malloc(len * sizeof(char) + 1);
	strcpy(*target, input);
}

loglevel parse_loglevel_val(const char *input) {
	if (strcmp(input, "LOG_DEBUG") == 0) {
		return LOG_DEBUG;
	}

	if (strcmp(input, "LOG_INFO") == 0) {
		return LOG_INFO;
	}

	if (strcmp(input, "LOG_MESSAGE") == 0) {
		return LOG_MESSAGE;
	}

	if (strcmp(input, "LOG_WARNING") == 0) {
		return LOG_WARNING;
	}

	if (strcmp(input, "LOG_ERROR") == 0) {
		return LOG_ERROR;
	}

	if (strcmp(input, "LOG_FAILURE") == 0) {
		return LOG_FAILURE;
	}
	// ERROR
	return -1;
}

void parse_simple_logger_console(const char **attr) {
	int i = 0;
	while (attr[i] && attr[i + 1]) {
		if (strcmp(attr[i], "enabled") == 0) {
			config.util.logging.simpleLogger.console.enabled =
				parse_bool(attr[i + 1]);
		}

		if (strcmp(attr[i], "loglevel") == 0) {
			config.util.logging.simpleLogger.console.logLevel =
				parse_loglevel_val(attr[i + 1]);
		}

		i = i + 2;
	}

}

void parse_simple_logger_file(const char **attr) {
	int i = 0;
	while (attr[i] && attr[i + 1]) {
		if (strcmp(attr[i], "enabled") == 0) {
			config.util.logging.simpleLogger.file.enabled =
				parse_bool(attr[i + 1]);
		}

		if (strcmp(attr[i], "filename") == 0) {
			parse_string(attr[i + 1],
						 &config.util.logging.simpleLogger.file.fileName);
		}

		if (strcmp(attr[i], "loglevel") == 0) {
			config.util.logging.simpleLogger.file.logLevel =
				parse_loglevel_val(attr[i + 1]);
		}

		if (strcmp(attr[i], "withtime") == 0) {
			config.util.logging.simpleLogger.file.withTime =
				parse_bool(attr[i + 1]);
		}

		if (strcmp(attr[i], "verbose") == 0) {
			config.util.logging.simpleLogger.file.verbose =
				parse_bool(attr[i + 1]);
		}

		if (strcmp(attr[i], "threadid") == 0) {
			config.util.logging.simpleLogger.file.threadId =
				parse_bool(attr[i + 1]);
		}

		i = i + 2;
	}
}

void parse_thread_mgmt_threads(const char **attr) {
	int i = 0;
	while (attr[i] && attr[i + 1]) {
		if (strcmp(attr[i], "maxamount") == 0) {
			config.util.threadManagement.threads.maxAmount =
				parse_int(attr[i + 1]);
		}

		i = i + 2;
	}
}

void parse_core_events_dispatcher(const char **attr) {
	int i = 0;
	while (attr[i] && attr[i + 1]) {
		if (strcmp(attr[i], "maxcalls") == 0) {
			config.core.events.dispatcher.maxCalls =
				parse_int(attr[i + 1]);
		}

		if (strcmp(attr[i], "maxevents") == 0) {
			config.core.events.dispatcher.maxEvents =
				parse_int(attr[i + 1]);
		}

		i = i + 2;
	}
}

void parse_remote_xmlrpc_client(const char **attr) {
	int i = 0;
	while (attr[i] && attr[i + 1]) {
		if (strcmp(attr[i], "name") == 0) {
			parse_string(attr[i + 1],
						 &config.remote.callback.xmlrpcClient.name);
		}

		if (strcmp(attr[i], "version") == 0) {
			parse_string(attr[i + 1],
						 &config.remote.callback.xmlrpcClient.version);
		}

		i = i + 2;
	}
}

void parse_remote_xmlrpc_server(const char **attr) {
	int i = 0;
	while (attr[i] && attr[i + 1]) {
		if (strcmp(attr[i], "port") == 0) {
			config.remote.server.xmlrpcServer.port =
				parse_int(attr[i + 1]);
		}

		if (strcmp(attr[i], "registerprefix") == 0) {
			parse_string(attr[i + 1],
						 &config.remote.server.xmlrpcServer.
						 registerPrefix);
		}

		if (strcmp(attr[i], "logfilename") == 0) {
			parse_string(attr[i + 1],
						 &config.remote.server.xmlrpcServer.logFileName);
		}

		i = i + 2;
	}
}

void parse_sipstack_exosip_adapter(const char **attr) {
	int i = 0;
	while (attr[i] && attr[i + 1]) {
		if (strcmp(attr[i], "port") == 0) {
			config.sipstack.eXosipAdapter.port = parse_int(attr[i + 1]);
		}

		i = i + 2;
	}
}

void parse_account(const char **attr) {
	struct account *cur_acc;
	int i = 0;

	cur_acc = (struct account *) malloc(sizeof(struct account));

	while (attr[i] && attr[i + 1]) {
		if (strcmp(attr[i], "name") == 0) {
			parse_string(attr[i + 1], &cur_acc->name);
		}

		if (strcmp(attr[i], "username") == 0) {
			parse_string(attr[i + 1], &cur_acc->username);
		}

		if (strcmp(attr[i], "domain") == 0) {
			parse_string(attr[i + 1], &cur_acc->domain);
		}

		if (strcmp(attr[i], "authusername") == 0) {
			parse_string(attr[i + 1], &cur_acc->authusername);
		}

		if (strcmp(attr[i], "password") == 0) {
			parse_string(attr[i + 1], &cur_acc->password);
		}

		if (strcmp(attr[i], "displayname") == 0) {
			parse_string(attr[i + 1], &cur_acc->displayname);
		}

		if (strcmp(attr[i], "outboundproxy") == 0) {
			parse_string(attr[i + 1], &cur_acc->outboundproxy);
		}

		if (strcmp(attr[i], "registrar") == 0) {
			parse_string(attr[i + 1], &cur_acc->registrar);
		}

		if (strcmp(attr[i], "autoregister") == 0) {
			cur_acc->autoregister = parse_bool(attr[i + 1]);
		}

		if (strcmp(attr[i], "id") == 0) {
			cur_acc->id = parse_int(attr[i + 1]);
		}

		i = i + 2;
	}

	add_node(cur_acc);
}

void parse_core_sipoutput_registrar_mgr(const char **attr) {
	int i = 0;
	while (attr[i] && attr[i + 1]) {
		if (strcmp(attr[i], "expire") == 0) {
			config.core.sipOutput.registrarManager.expire =
				parse_int(attr[i + 1]);
		}

		if (strcmp(attr[i], "preexpirerange") == 0) {
			config.core.sipOutput.registrarManager.preExpireRange =
				parse_int(attr[i + 1]);
		}

		if (strcmp(attr[i], "timeout") == 0) {
			config.core.sipOutput.registrarManager.timeout =
				parse_int(attr[i + 1]);
		}

		i = i + 2;
	}
}

void parse_accounts_account_mgmt(const char **attr) {
	int i = 0;
	while (attr[i] && attr[i + 1]) {
		if (strcmp(attr[i], "maxaccountidamount") == 0) {
			config.accounts.accountManagement.maxAccountIdAmount =
				parse_int(attr[i + 1]);
		}

		if (strcmp(attr[i], "maxvaluelength") == 0) {
			config.accounts.accountManagement.maxValueLength =
				parse_int(attr[i + 1]);
		}

		i = i + 2;
	}
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
void cr_elem_start(void *data, const char *name, const char **attr) {
	if (strcmp(name, "config") == 0) {
		withinConfig = 1;
		return;
	}

	if (withinConfig && strcmp(name, "util") == 0) {
		withinUtil = 1;
		return;
	}

	if (withinUtil && strcmp(name, "logging") == 0) {
		withinLogging = 1;
		return;
	}

	if (withinLogging && strcmp(name, "simplelogger") == 0) {
		withinSimpleLogger = 1;
		return;
	}

	if (withinSimpleLogger && strcmp(name, "console") == 0) {
		withinConsole = 1;

		parse_simple_logger_console(attr);

		return;
	}

	if (withinSimpleLogger && strcmp(name, "file") == 0) {
		withinFile = 1;

		parse_simple_logger_file(attr);
		return;
	}

	if (withinUtil && strcmp(name, "threadmanagement") == 0) {
		withinThreadManagement = 1;
		return;
	}

	if (withinThreadManagement && strcmp(name, "threads") == 0) {
		withinThreads = 1;

		parse_thread_mgmt_threads(attr);
		return;
	}

	if (withinConfig && strcmp(name, "core") == 0) {
		withinCore = 1;
		return;
	}

	if (withinCore && strcmp(name, "events") == 0) {
		withinEvents = 1;
		return;
	}

	if (withinEvents && strcmp(name, "dispatcher") == 0) {
		withinDispatcher = 1;

		parse_core_events_dispatcher(attr);
		return;
	}

	if (withinCore && strcmp(name, "sipoutput") == 0) {
		withinSipOutput = 1;
		return;
	}

	if (withinSipOutput && strcmp(name, "registrarmanager") == 0) {
		withinRegistrarManager = 1;

		parse_core_sipoutput_registrar_mgr(attr);
		return;
	}

	if (withinConfig && strcmp(name, "remote") == 0) {
		withinRemote = 1;
		return;
	}

	if (withinRemote && strcmp(name, "callback") == 0) {
		withinCallback = 1;
		return;
	}

	if (withinCallback && strcmp(name, "xmlrpcclient") == 0) {
		withinXmlrpcClient = 1;

		parse_remote_xmlrpc_client(attr);
		return;
	}

	if (withinRemote && strcmp(name, "server") == 0) {
		withinServer = 1;
		return;
	}

	if (withinServer && strcmp(name, "xmlrpcserver") == 0) {
		withinXmlrpcServer = 1;

		parse_remote_xmlrpc_server(attr);
		return;
	}

	if (withinConfig && strcmp(name, "sipstack") == 0) {
		withinSipstack = 1;
		return;
	}

	if (withinSipstack && strcmp(name, "exosipadapter") == 0) {
		withinExosipAdapter = 1;

		parse_sipstack_exosip_adapter(attr);
		return;
	}

	if (withinConfig && strcmp(name, "accounts") == 0) {
		withinAccounts = 1;
		return;
	}

	if (withinAccounts && strcmp(name, "list") == 0) {
		withinList = 1;
		return;
	}

	if (withinList && strcmp(name, "account") == 0) {
		parse_account(attr);
		return;
	}

	if (withinAccounts && strcmp(name, "accountmanagement") == 0) {
		withinAccountManagement = 1;

		parse_accounts_account_mgmt(attr);
		return;
	}

}

/**
	This function is called by the eXpat parser, if an XML element closing tag 
	has been discovered. 
    
    @param void* data User data
    @param char* name Name of the XML element
*/
void cr_elem_end(void *data, const char *name) {
	if (strcmp(name, "config") == 0) {
		withinConfig = 0;
		return;
	}

	if (withinConfig && strcmp(name, "util") == 0) {
		withinUtil = 0;
		return;
	}

	if (withinUtil && strcmp(name, "logging") == 0) {
		withinLogging = 0;
		return;
	}

	if (withinLogging && strcmp(name, "simplelogger") == 0) {
		withinSimpleLogger = 0;
		return;
	}

	if (withinSimpleLogger && strcmp(name, "console") == 0) {
		withinConsole = 0;
		return;
	}

	if (withinSimpleLogger && strcmp(name, "file") == 0) {
		withinFile = 0;
		return;
	}

	if (withinUtil && strcmp(name, "threadmanagement") == 0) {
		withinThreadManagement = 0;
		return;
	}

	if (withinThreadManagement && strcmp(name, "threads") == 0) {
		withinThreads = 0;
		return;
	}

	if (withinConfig && strcmp(name, "core") == 0) {
		withinCore = 0;
		return;
	}

	if (withinCore && strcmp(name, "events") == 0) {
		withinEvents = 0;
		return;
	}

	if (withinEvents && strcmp(name, "dispatcher") == 0) {
		withinDispatcher = 0;
		return;
	}

	if (withinCore && strcmp(name, "sipoutput") == 0) {
		withinSipOutput = 0;
		return;
	}

	if (withinSipOutput && strcmp(name, "registrarmanager") == 0) {
		withinRegistrarManager = 0;
		return;
	}

	if (withinConfig && strcmp(name, "remote") == 0) {
		withinRemote = 0;
		return;
	}

	if (withinRemote && strcmp(name, "callback") == 0) {
		withinCallback = 0;
		return;
	}

	if (withinCallback && strcmp(name, "xmlrpcclient") == 0) {
		withinXmlrpcClient = 0;
		return;
	}

	if (withinRemote && strcmp(name, "server") == 0) {
		withinServer = 0;
		return;
	}

	if (withinServer && strcmp(name, "xmlrpcserver") == 0) {
		withinXmlrpcServer = 0;
		return;
	}

	if (withinConfig && strcmp(name, "sipstack") == 0) {
		withinSipstack = 0;
		return;
	}

	if (withinSipstack && strcmp(name, "exosipadapter") == 0) {
		withinExosipAdapter = 0;
		return;
	}

	if (withinConfig && strcmp(name, "accounts") == 0) {
		withinAccounts = 0;
		return;
	}

	if (withinAccounts && strcmp(name, "list") == 0) {
		withinList = 0;
		return;
	}

	if (withinAccounts && strcmp(name, "accountmanagement") == 0) {
		withinAccountManagement = 0;
		return;
	}

}

/**
	Initialize list of accounts by parsing an XML file.
*/
int cr_init(const char *fileName) {
	int result;

	withinAccounts = 0;
	withinCore = 0;
	withinRemote = 0;
	withinSipstack = 0;
	withinUtil = 0;
	withinLogging = 0;
	withinSimpleLogger = 0;
	withinConsole = 0;
	withinFile = 0;
	withinThreadManagement = 0;
	withinThreads = 0;
	withinServer = 0;
	withinCallback = 0;
	withinXmlrpcServer = 0;
	withinXmlrpcClient = 0;
	withinEvents = 0;
	withinDispatcher = 0;
	withinList = 0;
	withinExosipAdapter = 0;
	withinAccountManagement = 0;
	withinSipOutput = 0;
	withinRegistrarManager = 0;

	int xmlfile = open(fileName, O_RDONLY);
	XML_Parser parser = XML_ParserCreate(NULL);

	XML_SetElementHandler(parser, cr_elem_start, cr_elem_end);
	for (;;) {
		int bytes_read;
		void *buf = XML_GetBuffer(parser, BUFFSIZE);

		if (buf == NULL) {
			// if memory could not be allocated
			printf(CONFIG_MSG_PREFIX "Buffer allocation failed.\n");
			result = 0;
			break;
		}

		bytes_read = read(xmlfile, buf, BUFFSIZE);

		if (bytes_read < 0) {
			// if file could not be read
			printf(CONFIG_MSG_PREFIX
				   "No file found or file reading error.\n");
			result = 0;
			break;
		}

		if (!XML_ParseBuffer(parser, bytes_read, bytes_read == 0)) {
			// if XML document is not well-formed
			printf(CONFIG_MSG_PREFIX "XML parse error in line %d: %s\n",
				   XML_GetCurrentLineNumber(parser),
				   XML_ErrorString(XML_GetErrorCode(parser)));
			result = 0;
			break;
		}
		// if end of file reached
		if (bytes_read == 0) {
			result = 1;
			break;
		}
	}
	XML_ParserFree(parser);
	close(xmlfile);

	// explicitly initializing URL, because it is not read from config
	config.remote.callback.guiCallback.guiURL = NULL;

	return result;
}

int cr_destroy() {
	if (config.util.logging.simpleLogger.file.fileName) {
		free(config.util.logging.simpleLogger.file.fileName);
	}
	if (config.remote.callback.xmlrpcClient.name) {
		free(config.remote.callback.xmlrpcClient.name);
	}
	if (config.remote.callback.xmlrpcClient.version) {
		free(config.remote.callback.xmlrpcClient.version);
	}
	if (config.remote.server.xmlrpcServer.registerPrefix) {
		free(config.remote.server.xmlrpcServer.registerPrefix);
	}
	if (config.remote.server.xmlrpcServer.logFileName) {
		free(config.remote.server.xmlrpcServer.logFileName);
	}
	// if unregisterGui was not called:
	if (config.remote.callback.guiCallback.guiURL) {
		free(config.remote.callback.guiCallback.guiURL);
	}
	del_all_nodes();
	return 1;
}
