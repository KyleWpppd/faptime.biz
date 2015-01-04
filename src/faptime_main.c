#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "faptime_table.h"

/* This flag set by the '--verbose' option */
static int verbose_flag;

int main(int argc, char *argv[]) {
    int c;
    int dump_table = 0;
    int new_table = 0;
    char *table_name = NULL;
    int table_base = 64;
    while (1) {
        static struct option long_options[]=
            {
                {"verbose", no_argument, &verbose_flag, 1},
                {"dump-table", no_argument, 0, 'O'},
                {"new-table", optional_argument, 0, 'n'},
                {"table", required_argument, 0, 'i'},
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
        case 'i':
            table_name = optarg;
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
