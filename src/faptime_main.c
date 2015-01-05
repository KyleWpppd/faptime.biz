#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <limits.h>
#include <libgen.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "faptime_table.h"


#define DEFAULT_CONFIG_FILE "faptime.ini"
/* This flag set by the '--verbose' option */
static int verbose_flag;

static char* default_config_file_path(char *argv0) {
	char *result = NULL;
	char *directory = NULL;
	char *exe_path;
	if (NULL == (exe_path = realpath(argv0, NULL)) ||
		NULL == (directory = strdup(dirname(exe_path)))) {
		goto cleanup;
	}

	size_t config_path_len = strlen(directory) + strlen("/") + strlen(DEFAULT_CONFIG_FILE) + 1;
	char *config_path = calloc(config_path_len, sizeof *config_path);
	if (config_path == NULL) {
		goto cleanup;
	}

	strlcat(config_path, directory, config_path_len);
	strlcat(config_path, "/", config_path_len);
	strlcat(config_path, DEFAULT_CONFIG_FILE, config_path_len);
	result = config_path;

 cleanup:
	free(exe_path);
	free(directory);

	return result;
}

int main(int argc, char *argv[]) {
	int c;
	int dump_table = 0;
	int new_table = 0;
	char *table_name = NULL;
	int table_base = 64;
	const char *config_file_path = default_config_file_path(argv[0]);
	while (1) {
		static struct option long_options[]=
			{
				{"verbose", no_argument, &verbose_flag, 1},
				{"dump-table", no_argument, 0, 'O'},
				{"new-table", optional_argument, 0, 'n'},
				{"config-file", required_argument, 0, 'c'},
				{0, 0, 0, 0}
			};


		int option_index = 0;

		c = getopt_long(argc, argv, "dn::", long_options, &option_index);

		if (-1 == c) {
			break;
		}

		switch (c) {
		case 0:
			break;
		case 'O':
			dump_table = 1;
			break;
		case 'c':
			config_file_path = optarg;
			break;
		case 'n':
			new_table = 1;
			if (optarg) {
				table_base = atoi(optarg);
			}
			break;
		case '?':
		default:
			fprintf(stderr, "Invalid option\n");
			abort();
		}
	}

	if (new_table && table_name) {
		fprintf(stderr, "Cannot create table and specify input table.\n");
		exit(1);
	}

	struct faptime_table *ft = NULL;
	struct stat st;

	if (config_file_path == NULL ||
		-1 == stat(config_file_path, &st) ||
		!S_ISREG(st.st_mode)) {
		fprintf(stderr, "Invalid config file at path: %s\n", config_file_path ? config_file_path : "Unknown (bad pointer!)");
		exit(2);
	}


	if (table_name) {
		ft = faptime_table_from_file(table_name);
		if (!ft || !faptime_set_global_table(ft)) {
			/* @FIXME, output needs to be gated by --verbose flag */
			fprintf(stderr, "Unable to read table\n");
			abort();
		}
	} else if (new_table) {
		ft = faptime_random_table(table_base);
	} {

	}


	if (dump_table) {
		faptime_print_table(ft);
		exit(0);
	}
	return 99;
}
