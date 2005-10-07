#ifndef HCONFIG_READER_USED
#define HCONFIG_READER_USED

#define CONFIG_MSG_PREFIX "[config reader] "

int cr_init(const char *fileName);
int cr_destroy();

#endif
