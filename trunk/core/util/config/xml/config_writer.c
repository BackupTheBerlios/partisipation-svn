#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <expat.h>

#include <lib/genx/genx.h>
#include <util/config/xml/config_writer.h>
#include <util/config/globals.h>
#include <util/list/list.h>
#include <util/logging/logger.h>

int save_on_exit = 0;
char *xml_file_name;

/**
	Prints the value of an integer to a string and returns its pointer.
	It is your responsibility to free memory this pointer refers to.
	
	@param int n integer value to convert to a string
	@return pointer to string
 */
char *int_to_str(int n) {
	char *str = (char *) malloc(20);
	snprintf(str, 10, "%d", n);
	return str;
}

/**
	This function converts log levels into strings.
	
	@param loglevel lev log level to be converted
	@return string representation of this log level
 */
const char *loglevel_to_str(loglevel lev) {

	if (lev == LOG_DEBUG)
		return "LOG_DEBUG";
	else if (lev == LOG_INFO)
		return "LOG_INFO";
	else if (lev == LOG_MESSAGE)
		return "LOG_MESSAGE";
	else if (lev == LOG_WARNING)
		return "LOG_WARNING";
	else if (lev == LOG_ERROR)
		return "LOG_ERROR";
	else
		return "LOG_FAILURE";
}

/**
	This function converts boolean values into their string representation.
	
	@param int b boolean value
	@return its string representation 
 */
char *bool_to_str(int b) {

	if (b)
		return "true";
	else
		return "false";
}

/**
 	This function writes configuration data to XML file specified during initialization. 
 	
	@return 1 on success, 0 on failure
 */
int cw_save_config() {

	int result = 1;
	FILE *xmlfile = fopen(xml_file_name, "w");

	if (xmlfile) {

		fprintf(xmlfile,
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
		fprintf(xmlfile,
				"<!DOCTYPE config SYSTEM \"./core_config.dtd\">\n");
		genxWriter w = genxNew(NULL, NULL, NULL);

		genxStartDocFile(w, xmlfile);

		genxStartElementLiteral(w, NULL, "config");
		genxAddText(w, "\n\t");

		// -------------- ACCOUNTS configuration ----------------

		genxStartElementLiteral(w, NULL, "accounts");

		genxAddText(w, "\n\t\t");
		genxStartElementLiteral(w, NULL, "accountmanagement");
		char *maxAccountIdAmount =
			int_to_str(config.accounts.accountManagement.
					   maxAccountIdAmount);
		genxAddAttributeLiteral(w, NULL, "maxaccountidamount",
								maxAccountIdAmount);
		free(maxAccountIdAmount);
		char *maxValueLength =
			int_to_str(config.accounts.accountManagement.maxValueLength);
		genxAddAttributeLiteral(w, NULL, "maxvaluelength", maxValueLength);
		free(maxValueLength);
		genxEndElement(w);		// </accountmanagement>

		genxAddText(w, "\n\t\t");
		genxStartElementLiteral(w, NULL, "list");

		struct node *tmp = head;

		// continue until the end of the list
		while (tmp) {

			// get current account
			struct account *acc = tmp->acc;

			genxAddText(w, "\n\t\t\t");
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
			genxAddAttributeLiteral(w, NULL, "autoregister",
									bool_to_str(acc->autoregister));

			genxEndElement(w);	// </account>

			tmp = tmp->next;

		}

		genxAddText(w, "\n\t\t");
		genxEndElement(w);		// </list>
		genxAddText(w, "\n\t");
		genxEndElement(w);		// </accounts>

		// ------------------- CORE configuration -------------------

		genxAddText(w, "\n\t");
		genxStartElementLiteral(w, NULL, "core");
		genxAddText(w, "\n\t\t");
		genxStartElementLiteral(w, NULL, "events");

		genxAddText(w, "\n\t\t\t");
		genxStartElementLiteral(w, NULL, "dispatcher");
		char *maxCalls =
			int_to_str(config.core.events.dispatcher.maxCalls);
		genxAddAttributeLiteral(w, NULL, "maxcalls", maxCalls);
		free(maxCalls);
		char *maxEvents =
			int_to_str(config.core.events.dispatcher.maxEvents);
		genxAddAttributeLiteral(w, NULL, "maxevents", maxEvents);
		free(maxEvents);
		genxEndElement(w);		// </dispatcher>
		genxAddText(w, "\n\t\t");
		genxEndElement(w);		// </events>

		genxAddText(w, "\n\t\t");
		genxStartElementLiteral(w, NULL, "sipoutput");

		genxAddText(w, "\n\t\t\t");
		genxStartElementLiteral(w, NULL, "registrarmanager");
		char *expire =
			int_to_str(config.core.sipOutput.registrarManager.expire);
		genxAddAttributeLiteral(w, NULL, "expire", expire);
		free(expire);
		char *preExpireRange =
			int_to_str(config.core.sipOutput.registrarManager.
					   preExpireRange);
		genxAddAttributeLiteral(w, NULL, "preexpirerange", preExpireRange);
		free(preExpireRange);
		char *timeout =
			int_to_str(config.core.sipOutput.registrarManager.timeout);
		genxAddAttributeLiteral(w, NULL, "timeout", timeout);
		free(timeout);
		genxEndElement(w);		// </registrarmanager>

		genxAddText(w, "\n\t\t");
		genxEndElement(w);		// </sipoutput>

		genxAddText(w, "\n\t");
		genxEndElement(w);		// </core>

		// ----------------- REMOTE configuration -----------------

		genxAddText(w, "\n\t");
		genxStartElementLiteral(w, NULL, "remote");
		genxAddText(w, "\n\t\t");
		genxStartElementLiteral(w, NULL, "callback");

		genxAddText(w, "\n\t\t\t");
		genxStartElementLiteral(w, NULL, "xmlrpcclient");
		genxAddAttributeLiteral(w, NULL, "name",
								config.remote.callback.xmlrpcClient.name);
		genxAddAttributeLiteral(w, NULL, "version",
								config.remote.callback.xmlrpcClient.
								version);
		genxEndElement(w);		// </xmlrpcclient>

		genxAddText(w, "\n\t\t");
		genxEndElement(w);		// </callback>

		genxAddText(w, "\n\t\t");
		genxStartElementLiteral(w, NULL, "server");
		genxAddText(w, "\n\t\t\t");
		genxStartElementLiteral(w, NULL, "xmlrpcserver");
		genxAddAttributeLiteral(w, NULL, "logfilename",
								config.remote.server.xmlrpcServer.
								logFileName);
		char *server_port =
			int_to_str(config.remote.server.xmlrpcServer.port);
		genxAddAttributeLiteral(w, NULL, "port", server_port);
		free(server_port);
		genxAddAttributeLiteral(w, NULL, "registerprefix",
								config.remote.server.xmlrpcServer.
								registerPrefix);
		genxEndElement(w);		// </xmlrpcserver>
		genxAddText(w, "\n\t\t");
		genxEndElement(w);		// </server>

		genxAddText(w, "\n\t");
		genxEndElement(w);		// </remote>

		// --------------- SIPSTACK configuration -------------------

		genxAddText(w, "\n\t");
		genxStartElementLiteral(w, NULL, "sipstack");
		genxAddText(w, "\n\t\t");
		genxStartElementLiteral(w, NULL, "exosipadapter");
		char *exosip_port = int_to_str(config.sipstack.eXosipAdapter.port);
		genxAddAttributeLiteral(w, NULL, "port", exosip_port);
		free(exosip_port);
		genxEndElement(w);		// </exosipadapter>
		genxAddText(w, "\n\t");
		genxEndElement(w);		// </sipstack>

		// --------------- UTIL configuration -------------------

		genxAddText(w, "\n\t");
		genxStartElementLiteral(w, NULL, "util");
		genxAddText(w, "\n\t\t");
		genxStartElementLiteral(w, NULL, "logging");
		genxAddText(w, "\n\t\t\t");
		genxStartElementLiteral(w, NULL, "simplelogger");

		genxAddText(w, "\n\t\t\t\t");
		genxStartElementLiteral(w, NULL, "console");	// <console>
		if (config.util.logging.simpleLogger.console.enabled)
			genxAddAttributeLiteral(w, NULL, "enabled", "true");
		else
			genxAddAttributeLiteral(w, NULL, "enabled", "false");
		genxAddAttributeLiteral(w, NULL, "loglevel",
								loglevel_to_str(config.util.logging.
												simpleLogger.console.
												logLevel));
		genxEndElement(w);		// </console>

		genxAddText(w, "\n\t\t\t\t");
		genxStartElementLiteral(w, NULL, "file");	// <file>
		genxAddAttributeLiteral(w, NULL, "enabled",
								bool_to_str(config.util.logging.
											simpleLogger.file.enabled));
		genxAddAttributeLiteral(w, NULL, "loglevel",
								loglevel_to_str(config.util.logging.
												simpleLogger.file.
												logLevel));
		genxAddAttributeLiteral(w, NULL, "filename",
								config.util.logging.simpleLogger.file.
								fileName);
		genxAddAttributeLiteral(w, NULL, "withtime",
								bool_to_str(config.util.logging.
											simpleLogger.file.withTime));
		genxAddAttributeLiteral(w, NULL, "verbose",
								bool_to_str(config.util.logging.
											simpleLogger.file.verbose));
		genxAddAttributeLiteral(w, NULL, "threadid",
								bool_to_str(config.util.logging.
											simpleLogger.file.threadId));
		genxEndElement(w);		// </file>

		genxAddText(w, "\n\t\t\t");
		genxEndElement(w);		// </simplelogger>
		genxAddText(w, "\n\t\t");
		genxEndElement(w);		// </logging>

		genxAddText(w, "\n\t\t");
		genxStartElementLiteral(w, NULL, "threadmanagement");
		genxAddText(w, "\n\t\t\t");
		genxStartElementLiteral(w, NULL, "threads");	// <threads>
		char *maxAmount =
			int_to_str(config.util.threadManagement.threads.maxAmount);
		genxAddAttributeLiteral(w, NULL, "maxamount", maxAmount);
		free(maxAmount);
		genxEndElement(w);		// </threads>
		genxAddText(w, "\n\t\t");
		genxEndElement(w);		// </threadmanagement>

		genxAddText(w, "\n\t");
		genxEndElement(w);		// </util>
		genxAddText(w, "\n");
		genxEndElement(w);		// </config>
		genxEndDocument(w);		// done

	} else {
		LOG_ERROR(CONFIG_WRITER_MSG_PREFIX "I/O error for file %s",
				  xml_file_name);
		result = 0;
	}
	// close file
	fclose(xmlfile);

	return result;
}

/**
	This function is to be called before shutdown. If config_writer was initialized with 
	saveCfgOnExit flag set to 1, the configuration is automatically saved.
	
	@return always 1
 */
int cw_destroy() {
	if (save_on_exit) {
		cw_save_config();
	}
	if (xml_file_name) {
		free(xml_file_name);
	}
	return 1;
}

/**
	This function Initializes config_writer.
	
	@param char *fileName path of XML file to be written
	@param saveCfgOnExit flag value: indicates if data should be auto-saved on shutdown
	@return whether initializing config_writer succeeded (boolean)
*/
int cw_init(const char *fileName, int saveCfgOnExit) {
	if (!fileName) {
		// file name is NULL
		return 0;
	}
	xml_file_name = (char *) malloc(strlen(fileName) * sizeof(char) + 1);
	strcpy(xml_file_name, fileName);
	save_on_exit = saveCfgOnExit;

	return 1;
}
