#ifndef HAVE_FAPTIME_CONFIG_H
#define HAVE_FAPTIME_CONFIG_H

#include "iniparser.h"
#include "faptime_table.h"

#define FT_CONFIG_ERROR -1
#define FT_CONFIG_OK 0
#define FT_BAD_CONFIG 1
#define FT_NO_TABLE 2
#define FT_BAD_TABLE 3
#define FT_NO_CONFIG 4

#define FT_CFGKEY_DEFAULT_URL_CHARS "urls:whitelist"
#define FAPTIME_DEFAULT_CONFIG_FILE "faptime.conf"

int faptime_config_parse(char *path);
int faptime_config_dump(void);

char *faptime_config_getstring(const char *key);
char *faptime_config_get_whitelist(void);
char *faptime_config_get_table(void);


int faptime_config_set_table(struct faptime_table *ft);
void faptime_config_free(void);
#endif
