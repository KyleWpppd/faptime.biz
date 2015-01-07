#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
extern int errno;

#include "faptime_config.h"
#include "faptime_table.h"

#define CFG_TABLE "urls:mapping"
#define CFG_WHITELIST "urls:whitelist"

/**
   @brief Attempts to parse the config file at the given path
   @param path	  The path of the config file.

   @return int returns FT_CONFIG_OK if the configuration file was successfully
   parsed, any other return value represents an error condition.
 */
static dictionary *config = NULL;
int faptime_config_parse(char *path) {
	if (config != NULL) {
		iniparser_freedict(config);
		config = NULL;
	}

	config = iniparser_load(path);
	if (!config) {
		/* This is the only real error here... */
		return FT_CONFIG_ERROR;
	}

	char *table = faptime_config_get_table();
	if (table == NULL) {
		return FT_NO_TABLE;
	}

	struct faptime_table *ft = faptime_table_from_string(table, NULL);
	free(table);

	if (!ft) {
		iniparser_freedict(config);
		config = NULL;
		return FT_BAD_TABLE;
	}

	faptime_set_global_table(ft);
	free(ft);
	ft = NULL;
	return FT_CONFIG_OK;
}

/**
   @brief Dumps the loaded config to stdout

   @return int Returns FT_CONFIG_OK if there was a config file (even if it was
   empty). If no config has been loaded, function returns FT_NO_CONFIG.
  */
int faptime_config_dump(void) {
	if (config == NULL) {
		return FT_NO_CONFIG;
	}

	iniparser_dump_ini(config, stdout);
	return FT_CONFIG_OK;
}

char *faptime_config_getstring(const char *key) {
	if (config == NULL) {
		errno = FT_NO_CONFIG;
		return NULL;
	}

	char default_val[] = "";
	char *result = iniparser_getstring(config, key, default_val);
	if (!result || result == default_val) {
		return NULL;
	}
	return strdup(result);
}

char *faptime_config_get_table(void) {
	return faptime_config_getstring(CFG_TABLE);
}

char *faptime_config_get_whitelist(void) {
	return faptime_config_getstring(CFG_WHITELIST);
}

int faptime_config_set_table(struct faptime_table *ft) {
	if (config == NULL) {
		return FT_NO_CONFIG;
	}
	const char *pretty_table = faptime_table_tostring(ft);
	if (0 != iniparser_set(config, CFG_TABLE, pretty_table)) {
		return FT_BAD_CONFIG;
	}
	return FT_CONFIG_OK;
}

void faptime_config_free(void) {
	if (config != NULL) {
		free(config);
		config = NULL;
	}
	return;
}
