#ifndef HAVE_FAPTIME_TABLE
#define HAVE_FAPTIME_TABLE

#include <sys/types.h>

#define FAPTIME_LOOKUP_TABLE_SIZE 128
#define FAPTIME_AVAILABLE_CHARACTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.-"
struct faptime_table {
    int len;
    char *table;
    /* Returns the faptime ordinal number for a character in a url. For example,
       with a table aXbYcZ, lookup['Z'] == 5. Elements that don't exist are set
       to -1. */
    int8_t lookup[FAPTIME_LOOKUP_TABLE_SIZE];
};

int faptime_random_table(int size, char *dst);
int faptime_save_table(char *dest_path, char *table, size_t table_len);
char * faptime_read_table(char *read_path, size_t maxlen, size_t *size_out);
int faptime_print_table(struct faptime_table *table);
int faptime_valid_table(char *table, size_t table_len, char* allowed);
struct faptime_table *faptime_table_from_string(char *table, size_t table_len, char *allowed);

#endif
