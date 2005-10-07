#ifndef HGLOBALS_USED
#define HGLOBALS_USED

#include <util/logging/logger.h>

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

#endif
