#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <util/logging/logger.h>
#include <util/config/config_reader.h>

const int MAX_LOG_MSG_LEN = 2048;

pthread_mutex_t logFileLock;

int logger_init() {
	int rc;

	rc = pthread_mutex_init(&logFileLock, NULL);
	if (rc != 0) {
		// ERROR
		return 0;
	}

	return 1;
}

int logger_destroy() {
	int rc;

	rc = pthread_mutex_destroy(&logFileLock);
	if (rc != 0) {
		// ERROR
		return 0;
	}

	return 1;
}

int ensure_newline(char *msg) {
	int l;
	int nlPresent;

	l = strlen(msg);
	nlPresent = 0;

	// check for newline at end of string if possible
	if (l > 0) {
		nlPresent = msg[l - 1] == '\n';
	}
	// no newline found: 
	if (!nlPresent) {
		if (l < MAX_LOG_MSG_LEN - 1) {
			strcat(msg, "\n");
			return 1;
		} else {
			// it is not possible to add a newline (error)
			return 0;
		}
	}

	return 1;
}

void log_message(loglevel lvl, const char *fmt, ...) {
	int rc;

	// format variable parameter list:
	va_list ap;
	char strtmp[MAX_LOG_MSG_LEN];
	va_start(ap, fmt);
	vsprintf(strtmp, fmt, ap);
	va_end(ap);

	// ensure line ends with newline, so next message won't be screwed up
	rc = ensure_newline(strtmp);
	if (!rc) {
		// ERROR
	}

	if (config.logging.simpleLogger.console.enabled
		&& lvl >= config.logging.simpleLogger.console.logLevel) {

		printf(strtmp);
	}

	if (config.logging.simpleLogger.file.enabled
		&& lvl >= config.logging.simpleLogger.file.logLevel) {

		pthread_mutex_lock(&logFileLock);
		FILE *f = fopen(config.logging.simpleLogger.file.fileName, "a");
		if (f == NULL) {
			// ERROR
			printf("error opening %s\n",
				   config.logging.simpleLogger.file.fileName);
			return;
		}

		if (config.logging.simpleLogger.file.withTime) {
			char timebuf[100];
			time_t t = time(NULL);
			strftime(timebuf, 100, "%d.%m.%Y - %H:%M:%S : ",
					 localtime(&t));
			fprintf(f, "%s", timebuf);
		}

		if (config.logging.simpleLogger.file.verbose) {
			switch (lvl) {
				case LOG_DEBUG:
					fprintf(f, "%s", "[DBG] : ");
					break;
				case LOG_INFO:
					fprintf(f, "%s", "[INF] : ");
					break;
				case LOG_MESSAGE:
					fprintf(f, "%s", "[MSG] : ");
					break;
				case LOG_WARNING:
					fprintf(f, "%s", "[WRN] : ");
					break;
				case LOG_ERROR:
					fprintf(f, "%s", "[ERR] : ");
					break;
				case LOG_FAILURE:
					fprintf(f, "%s", "[FLR] : ");
					break;
				default:
					fprintf(f, "%s", "[N/A] : ");
					break;
			}
		}

		if (config.logging.simpleLogger.file.threadId) {
			fprintf(f, "Thread ID <%d> : ", pthread_self());
		}

		fprintf(f, "%s", strtmp);

		rc = fclose(f);
		if (rc) {
			// ERROR
		}

		pthread_mutex_unlock(&logFileLock);
	}
}
