#ifndef HLOGGER_USED
#define HLOGGER_USED

typedef enum {
	LOG_DEBUG,
	LOG_INFO,
	LOG_MESSAGE,
	LOG_WARNING,
	LOG_ERROR,
	LOG_FAILURE
} loglevel;

int logger_init();

void log_message(loglevel lvl, const char *fmt, ...);

int logger_destroy();

#endif
