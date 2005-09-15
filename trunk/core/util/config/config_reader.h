#ifndef HCONFIG_READER_USED
#define HCONFIG_READER_USED

#include <util/logging/logger.h>

#define CONFIG_MSG_PREFIX "[config reader] "

typedef struct {
	int enabled;
	loglevel logLevel;
} consolecfg;

typedef struct {
	int enabled;
	char *fileName;
	loglevel logLevel;
	int withTime;
	int verbose;
	int threadId;
} filecfg;

typedef struct {
	consolecfg console;
	filecfg file;
} simpleloggercfg;

typedef struct {
	simpleloggercfg simpleLogger;
} loggingcfg;

typedef struct {
	loggingcfg logging;
} configcfg;

extern configcfg config;

int cr_init(const char *fileName);
int cr_destroy();

#endif
