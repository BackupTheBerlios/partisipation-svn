#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <expat.h>

#include <util/config/config_reader.h>

configcfg config;

const int BUFFSIZE = 1024;

int withinConfig;
int withinLogging;
int withinSimpleLogger;
int withinConsole;
int withinFile;

int parse_bool_val(const char *input) {
	if (strcmp(input, "true") == 0) {
		return 1;
	}

	if (strcmp(input, "false") == 0) {
		return 0;
	}
	// ERROR
	return -1;
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

void elem_start(void *data, const char *name, const char **attr) {
	if (strcmp(name, "config") == 0) {
		withinConfig = 1;
		return;
	}

	if (withinConfig && strcmp(name, "logging") == 0) {
		withinLogging = 1;
		return;
	}

	if (withinLogging && strcmp(name, "simplelogger") == 0) {
		withinSimpleLogger = 1;
		return;
	}

	if (withinSimpleLogger && strcmp(name, "console") == 0) {
		withinConsole = 1;

		int i = 0;
		while (attr[i] && attr[i + 1]) {
			if (strcmp(attr[i], "enabled") == 0) {
				config.logging.simpleLogger.console.enabled =
					parse_bool_val(attr[i + 1]);
			}

			if (strcmp(attr[i], "loglevel") == 0) {
				config.logging.simpleLogger.console.logLevel =
					parse_loglevel_val(attr[i + 1]);
			}

			i = i + 2;
		}

		return;
	}

	if (withinSimpleLogger && strcmp(name, "file") == 0) {
		withinFile = 1;

		int i = 0;
		while (attr[i] && attr[i + 1]) {
			if (strcmp(attr[i], "enabled") == 0) {
				config.logging.simpleLogger.file.enabled =
					parse_bool_val(attr[i + 1]);
			}

			if (strcmp(attr[i], "filename") == 0) {
				config.logging.simpleLogger.file.fileName =
					(char *) malloc(1024 * sizeof(char));
				strcpy(config.logging.simpleLogger.file.fileName,
					   attr[i + 1]);
			}

			if (strcmp(attr[i], "loglevel") == 0) {
				config.logging.simpleLogger.file.logLevel =
					parse_loglevel_val(attr[i + 1]);
			}

			if (strcmp(attr[i], "withtime") == 0) {
				config.logging.simpleLogger.file.withTime =
					parse_bool_val(attr[i + 1]);
			}

			if (strcmp(attr[i], "verbose") == 0) {
				config.logging.simpleLogger.file.verbose =
					parse_bool_val(attr[i + 1]);
			}

			if (strcmp(attr[i], "threadid") == 0) {
				config.logging.simpleLogger.file.threadId =
					parse_bool_val(attr[i + 1]);
			}

			i = i + 2;
		}
		return;
	}
}

void elem_end(void *data, const char *name) {
	if (strcmp(name, "config") == 0) {
		withinConfig = 0;
		return;
	}

	if (withinConfig && strcmp(name, "logging") == 0) {
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
}

/**
	Initialize list of accounts by parsing an XML file.
*/
int cr_init(const char *fileName) {
	int result;

	withinConfig = 0;
	withinLogging = 0;
	withinSimpleLogger = 0;
	withinConsole = 0;
	withinFile = 0;

//  printf(CONFIG_MSG_PREFIX "entering cr_init\n");

	int xmlfile = open(fileName, O_RDONLY);
	XML_Parser parser = XML_ParserCreate(NULL);

	XML_SetElementHandler(parser, elem_start, elem_end);
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

	if (result) {
		printf(CONFIG_MSG_PREFIX "parsed config successfully\n");
	}
//  printf(CONFIG_MSG_PREFIX "leaving cr_init\n");
	return result;
}

int cr_destroy() {
	// not used yet
	if (config.logging.simpleLogger.file.fileName) {
		free(config.logging.simpleLogger.file.fileName);
	}
	return 1;
}
