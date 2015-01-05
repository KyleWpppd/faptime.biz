#include <fcntl.h>

#include "faptime_config.h"
#include "faptime_table.h"

#define OK 0
#define BAD_CONFIG 1
#define NO_TABLE 2
#define BAD_TABLE 3

int faptime_config_iniparse(char *path) {
    dictionary *d = iniparser_load(path);
    if (!d) {
        return BAD_CONFIG;
    }
    char default_table[] = "";
    char *table = strdup(iniparser_getstring(d, "urls:table", default_table));
    if (table == default_table) {
        return NO_TABLE;
    }

    struct faptime_table *ft = faptime_table_from_string(table, strlen(table), NULL);
    if (!ft) {
        iniparser_freedict(d);
        d = NULL;
        return BAD_TABLE;
    }

    faptime_set_global_table(ft);
    free(ft);
    ft = NULL;
    iniparser_freedict(d);
    d = NULL;
    return OK;
}
