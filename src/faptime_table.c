#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <math.h>
#include <errno.h>
extern int errno;

#include "faptime_config.h"
#include "faptime_table.h"
#include "faptime_errors.h"
#include "faptime_urls.h"

static int write_table(int filedes, char *table, size_t table_len) {
	int file_ok = fcntl(filedes, F_GETFD);
	if (-1 == file_ok) {
		return -1;
	}

	return write(filedes, table, table_len);
}

static int close_filedes(int filedes) {
	int close_ok = 0;
	do {
		errno = 0;
		close_ok = close(filedes);
	} while ((-1 == close_ok) && (EINTR == errno));
	return close_ok;
}

static struct faptime_table * faptime_alloc_table() {
	faptime_table_t *ft = calloc(1, sizeof(*ft));
	ft->len = 0;
	ft->table = NULL;
    for (int i = 0; i < FAPTIME_LOOKUP_TABLE_SIZE; i++) {
        ft->lookup[i] = '\0';
    }
	/* memset(ft->lookup, FAPTIME_LOOKUP_INVALID, sizeof(ft->lookup)); */
	return ft;
}

char *shuffle_table(char *table) {
	size_t len = strlen(table);
	char *table_cpy = strdup(table);
	char *buf = NULL;

	if (table_cpy == NULL ||
		len < 2 ||
		NULL == (buf = calloc(len + 1, sizeof *buf))) {
		free(table_cpy);
		free(buf);
		return NULL;
	}

	srandom(time(NULL));
	size_t ct = 0, i = 0;
	while (ct < len) {
		do {
			/* Use 'random' rather than 'rand' because all bits in 'random' are usable. */
			i = random() % len;
		} while (! table_cpy[i]);
		buf[ct++] = table_cpy[i];
		table_cpy[i] = '\0';
	}
	buf[len] = '\0';
	assert(len == strlen(buf));
	free(table_cpy);
	return buf;
}

/**
	@brief Shuffles a copy of whitelisted characters to obtain an arbitrary base

	@param dst The destination of the shuffled table

	@param whitelist The allowable characters. A larger list means a larger
	arbitrary-based number system.

	@return int returns the length of the shuffled character array in dst or -1
	in case of an error.
*/
int faptime_random_table_from_whitelist(char *dst, char *whitelist) {
	size_t maxlen = strlen(FAPTIME_ALL_URL_CHARS);
	size_t whitelist_len = strnlen(whitelist, maxlen + 1);
	if (whitelist_len > maxlen) {
		return -1;
	}

	dst = shuffle_table(whitelist);
	if (dst == NULL) {
        return -1;
	}
    return strlen(dst);
}


int faptime_random_table_from_base(char **dst, size_t base) {
	/* It's not possible for a table to have more characters than the RFC-3986
	   legal characters, because then it would have repeats or illegals. */
	size_t max = strlen(FAPTIME_ALL_URL_CHARS);
	if (base > max) {
		return -1;
	}
	char *whitelist = calloc(base + 1, sizeof *whitelist);
	if (! whitelist) {
		return -1;
	}

	strncpy(whitelist, FAPTIME_ALL_URL_CHARS, base);
	*dst = shuffle_table(whitelist);
	assert(dst != NULL);
	free(whitelist);
    if (dst) {
        return strlen(*dst);
    }

	return -1;
}

void faptime_free_table(struct faptime_table *ft) {
	if (! ft) {
		return;
	}

	if (ft->table) {
		free(ft->table);
		ft->table = NULL;
	}

	free(ft);
	return;
}

struct faptime_table *faptime_random_table(char *whitelist) {
	char *tmp = NULL;
	if (faptime_random_table_from_whitelist(tmp, whitelist) == -1) {
		return NULL;
	}

	struct faptime_table *ft = faptime_table_from_string(tmp, whitelist);
	free(tmp);
	tmp = NULL;
	return ft;
}




int faptime_save_table(char *dest_path, char *table, size_t table_len) {
	int filedes = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	int t_errno = 0;
	if (filedes < 1) {
		return filedes;
	}
	int save_ok = write_table(filedes, table, table_len);
	/* Save the error to use after close call */
	if (-1 == save_ok) {
		t_errno = errno;
	}

	close_filedes(filedes);
	errno = t_errno;
	return save_ok;
}


void tabular_print(char *arr, int len, char type) {
	for (int i = 0; i < 10; i ++) {
		fprintf(stderr, "\t%d", i);
	}
	fprintf(stderr, "\n\n");

	char fmt[] = "\t%_";
	switch (type) {
	case 'c':
	case 'd':
		fmt[2] = type;
		break;
	default:
		abort();
	}

	for (int i = 0; i < len; i++) {
		if (i % 10 == 0) {
			fprintf(stderr, "%02d", i);
		}
		fprintf(stderr, fmt, arr[i]);
		if (i % 10 == 9) {
			fprintf(stderr, "\n");
		}
	}
	fprintf(stderr, "\n\n");
}

int faptime_print_table(struct faptime_table *ft) {
	if (!ft) {
		fprintf(stderr, "Invalid table!\n");
		return 0;
	}
	fprintf(stderr, "Table:\n");
	fprintf(stderr, "\t%d members\n", ft->len);
	tabular_print(ft->table, ft->len, 'c');
	/* Just create a temporary table rather than resort to nasty casts to jam
	   int8_t into a char */
	char tmplookup[FAPTIME_LOOKUP_TABLE_SIZE] = { '\0' };
	for (int i = 0; i < FAPTIME_LOOKUP_TABLE_SIZE; i++) {
		tmplookup[i] = ft->lookup[i];
	}
	tabular_print(tmplookup, FAPTIME_LOOKUP_TABLE_SIZE, 'd');
	return 1;
}

int faptime_valid_table(char *table, char* allowed) {
	if (table == NULL) {
		return 0;
	}

	size_t maxlen = strlen(FAPTIME_ALL_URL_CHARS);
	size_t table_len = strnlen(table, maxlen + 1);
	if (table_len > maxlen) {
		return 0;
	}

	char *tmpallowed;
	tmpallowed = allowed ? strdup(allowed) : faptime_config_getstring(FT_CFGKEY_DEFAULT_URL_CHARS);
	if (tmpallowed == NULL) {
		return 0;
	}

	/* Ensure no illegal characters */
	#define VALID 1
	#define INVALID 0
	int table_status = VALID;
	size_t illegal_idx = strspn(table, tmpallowed);

	if (table_len == illegal_idx && table_len <= strlen(tmpallowed)) {
        unsigned char seen[128] = { 0 };
		for (size_t i = 0; i < table_len; i++) {
            unsigned char c;
			c = table[i];
			/* Greater than 127 check so table doesn't have to fit all unsigned chars */
			if (c > 127 || seen[c]) {
				table_status = INVALID;
			}
			seen[c] = 1;
		}
	}
	free(tmpallowed);
	return table_status;
}


static void faptime_init_lookup_table(struct faptime_table *ft) {
	assert(ft->len > 0);
	for (size_t i = 0; i < FAPTIME_LOOKUP_TABLE_SIZE; i++) {
		ft->lookup[i] = -1;
	}

	for (size_t i = 0; i < (size_t)ft->len; i++) {
		unsigned char c = ft->table[i];
		ft->lookup[c] = i;
	}
}

int faptime_table_chartoi(const faptime_table_t *ft, const unsigned char c) {
    if (ft == NULL) {
        return -1;
    }
    if (c > (FAPTIME_LOOKUP_TABLE_SIZE - 1)) {
        return FAPTIME_LOOKUP_INVALID;
    }

    int val = ft->lookup[c];
    return val;
}


int faptime_table_antoi(const faptime_table_t *ft, const char *str, size_t strlen) {
    if (! strlen) {
        return 0;
    }

    int sum = 0;
    for (int i = strlen-1; i >= 0; i--) {
        char c = str[i];
        int val;
        double tmp;
        val = faptime_table_chartoi(ft, c);
        tmp = val * pow(strlen, i+1);
        /* This should be taking advantage of promotion here to get a larger
           range from double than we can from int. */
        if (tmp > INT_MAX || (sum + tmp) > INT_MAX) {
            return -1;
        }
        sum += tmp;
    }
    return sum;
}


struct faptime_table * faptime_table_from_string(char *table, char* allowed) {
	if (table == NULL || ! faptime_valid_table(table, allowed)) {
		return NULL;
	}

	struct faptime_table *ft = faptime_alloc_table();
	if (! ft) {
		return NULL;
	}

	ft->len = strlen(table);
	if (NULL == (ft->table = strdup(table))) {
		faptime_free_table(ft);
		return NULL;
	}

	faptime_init_lookup_table(ft);
	return ft;
}

struct faptime_table *faptime_table_copy(struct faptime_table *ft) {
	struct faptime_table *tmp = faptime_alloc_table();
	if (!tmp) {
		return NULL;
	}

	memcpy(tmp, ft, sizeof(*tmp));
	tmp->table = calloc(ft->len, sizeof(ft->table[0]));
	if (! tmp->table) {
		free(tmp);
		return NULL;
	}

	memcpy(tmp->table, ft->table, ft->len);
	return tmp;
}

char *faptime_table_tostring(struct faptime_table *ft) {
	if (ft == NULL || ft->table == NULL) {
		return NULL;
	}

	return strdup(ft->table);
}

static struct faptime_table *global_table = NULL;
struct faptime_table *faptime_get_global_table(void) {
	return global_table;
}

int faptime_set_global_table(struct faptime_table *ft) {
	faptime_free_table(global_table);
	global_table = faptime_table_copy(ft);
	return global_table ? 1 : 0;
}
