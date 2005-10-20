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
	int maxAmount;
} threadscfg;

typedef struct {
	threadscfg threads;
} threadmanagementcfg;

typedef struct {
	loggingcfg logging;
	threadmanagementcfg threadManagement;
} utilcfg;

typedef struct {
	char *guiURL;
} guicallbackcfg;

typedef struct {
	char *name;
	char *version;
} xmlrpcclientcfg;

typedef struct {
	guicallbackcfg guiCallback;
	xmlrpcclientcfg xmlrpcClient;
} callbackcfg;

typedef struct {
	int port;
	char *registerPrefix;
	char *logFileName;
} xmlrpcservercfg;

typedef struct {
	xmlrpcservercfg xmlrpcServer;
} servercfg;

typedef struct {
	callbackcfg callback;
	servercfg server;
} remotecfg;

typedef struct {
	int maxCalls;
	int maxEvents;
} dispatchercfg;

typedef struct {
	dispatchercfg dispatcher;
} eventscfg;

typedef struct {
	eventscfg events;
} corecfg;

typedef struct account {
	int id;
	char *name;
	char *username;
	char *domain;
	char *authusername;
	char *password;
	char *displayname;
	char *outboundproxy;
	char *registrar;
	int autoregister;
} typeaccount;

typedef struct node {
	struct account *acc;
	struct node *next;
	struct node *prev;
} typenode;

typedef struct {
	struct node list;
} accountscfg;

typedef struct {
	int port;
} exosipadaptercfg;

typedef struct {
	exosipadaptercfg eXosipAdapter;
} sipstackcfg;

typedef struct {
	accountscfg accounts;
	corecfg core;
	remotecfg remote;
	sipstackcfg sipstack;
	utilcfg util;
} configcfg;

extern configcfg config;

#endif
