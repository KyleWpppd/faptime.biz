#ifndef HAVE_FAPTIME_TABLE
#define HAVE_FAPTIME_TABLE

#include <sys/types.h>

#define FAPTIME_LOOKUP_TABLE_SIZE 128
#define FAPTIME_LOOKUP_INVALID -1
#define NEW_FAPTIME_TABLE {.len = 0, .table = NULL, .lookup = { 0 }}
typedef struct faptime_table {
	int len;
	char *table;
	/* Returns the faptime ordinal number for a character in a url. For example,
	   with a table aXbYcZ, lookup['Z'] == 5. Elements that don't exist are set
	   to -1. */
    char lookup[FAPTIME_LOOKUP_TABLE_SIZE];
} faptime_table_t;

void faptime_free_table(struct faptime_table *ft);

int faptime_random_table_from_base(char **dst, size_t base);
int faptime_random_table_from_whitelist(char *dst, char *whitelist);
struct faptime_table *faptime_random_table(char *whitelist);

int faptime_save_table(char *dest_path, char *table, size_t table_len);
struct faptime_table *faptime_table_from_string(char *table, char *allowed);

char *faptime_table_tostring(struct faptime_table *ft);
int faptime_print_table(struct faptime_table *table);
int faptime_valid_table(char *table, char* allowed);


struct faptime_table *faptime_table_copy(struct faptime_table *ft);
int faptime_set_global_table(struct faptime_table *ft);
struct faptime_table *faptime_get_global_table();

/* Faptime table conversions */
int faptime_table_chartoi(const faptime_table_t *ft, const unsigned char c);
int faptime_table_antoi(const faptime_table_t *ft, const char *str, size_t strlen);

#endif
