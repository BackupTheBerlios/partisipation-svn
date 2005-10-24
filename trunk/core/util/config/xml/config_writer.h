#ifndef HCONFIG_WRITER_USED
#define HCONFIG_WRITER_USED

#define CONFIG_WRITER_MSG_PREFIX "[config writer] "

int cw_init(const char *fileName, int saveCfgOnExit);
int cw_save_config();
int cw_destroy();

#endif
