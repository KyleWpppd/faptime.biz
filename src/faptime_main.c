#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>
#include <libgen.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#include "faptime_urls.h"
#include "faptime_table.h"
#include "faptime_config.h"


/* This flag set by the '--verbose' option */
static int verbose_flag;

static char* default_config_file_path(char *argv0) {
	char *result = NULL;
	char *directory = NULL;
	char *exe_path;
	if (NULL == (exe_path = realpath(argv0, NULL)) ||
		NULL == (directory = dirname(exe_path))) {
		goto cleanup;
	}

	char *config_path;
    if (-1 == asprintf(&config_path, "%s/%s", directory, FAPTIME_DEFAULT_CONFIG_FILE)) {
		goto cleanup;
	}
	result = config_path;

 cleanup:
	free(exe_path);
	return result;
}

#define DUMP_NONE 0
#define DUMP_CONFIG 1
#define DUMP_TABLE 2

int main(int argc, char *argv[]) {
	int dump = 0;
	int generate_table = 0;
	char *table_str = NULL;
	char *config_file_path = default_config_file_path(argv[0]);
	char *allowed_chars = NULL;
	while (1) {
        int c;
		static struct option long_options[]= {
			{"verbose", no_argument, &verbose_flag, 1},
			{"dump-table", no_argument, 0, '0'},
			{"dump-config", no_argument, 0, 'd'},
			{"generate-new-table", no_argument, 0, 'g'},
			{"allowed-characters", required_argument, 0, 'A'},
			{"config-file", required_argument, 0, 'c'},
			{"table", required_argument, 0, 't'},
			{0, 0, 0, 0}
		};

		int option_index = 0;

		c = getopt_long(argc, argv, "dcn::", long_options, &option_index);

		if (-1 == c) {
			break;
		}

		switch (c) {
		case 0:
			break;
		case 'A':
			allowed_chars = optarg;
			break;
		case 'O':
			dump = DUMP_TABLE;
			break;
		case 'c':
			config_file_path = optarg;
			break;
		case 'd':
			dump = DUMP_CONFIG;
		case 'g':
			generate_table = 1;
			break;
		case't':
			table_str = optarg;
            break;
		case '?':
		default:
			fprintf(stderr, "Invalid option\n");
			abort();
		}
	}

    if (!allowed_chars) {
        allowed_chars = strdup(FAPTIME_DEFAULT_URL_CHARS);
    }

	if (generate_table && table_str) {
		fprintf(stderr, "Cannot create table and specify input table.\n");
		exit(1);
	}

	struct faptime_table *ft = NULL;
	struct stat st;

	/* Fail if there is no config file */
	if (config_file_path == NULL ||
		-1 == stat(config_file_path, &st) ||
		!S_ISREG(st.st_mode)) {
		fprintf(stderr, "Invalid config file at path: %s\n", config_file_path ? config_file_path : "Unknown (bad pointer!)");
		free(config_file_path);
		exit(2);
	}

	/*  Fail if the config file has improper format */
	int error = 0;
	if ((error = faptime_config_parse(config_file_path)) == FT_CONFIG_ERROR) {
		fprintf(stderr, "Unable to read config at '%s'.\n", config_file_path);
		fprintf(stderr, "Error was: %d\n", error);
		exit(3);
	}

	if (faptime_url_is_named_group(allowed_chars)) {
		char *tmp = faptime_url_getgroup(allowed_chars, NULL);
        free(allowed_chars);
        allowed_chars = tmp;
	}


	if (table_str) {
		ft = faptime_table_from_string(table_str, allowed_chars);
	} else {
		char *table = faptime_config_get_table();
		char *whitelist = allowed_chars ? faptime_url_getgroup(allowed_chars, allowed_chars) : faptime_config_get_whitelist();
		if (! table) {
			fprintf(stderr, "Unable to retrieve table from config file at '%s'.\n"
					"Exiting.\n", config_file_path);
			exit(3);
		}
		ft = faptime_table_from_string(table, whitelist);
		free(table);
		free(whitelist);
	}

	if (generate_table) {
		/* We need to check this against the permitted characters in config */
		char *whitelist = allowed_chars ? allowed_chars : faptime_config_get_whitelist();
		ft = faptime_random_table(whitelist);
		faptime_config_set_table(ft);
	}

	if (!ft || !faptime_set_global_table(ft)) {
		/* @FIXME, output needs to be gated by --verbose flag */
		fprintf(stderr, "Unable to read table\n");
		abort();
	}

	if (dump == DUMP_CONFIG) {
		if (faptime_config_dump() != 0) {
			fprintf(stderr, "Failed to dump config!\n");
			exit(2);
		}
	} else if (dump == DUMP_TABLE) {
		faptime_print_table(ft);
		exit(0);
	}
	faptime_config_free();
	return 99;
}
