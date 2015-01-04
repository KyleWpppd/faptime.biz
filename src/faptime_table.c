#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
extern int errno;

#include "faptime_table.h"
#include "faptime_errors.h"

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
	struct faptime_table *ft = calloc(1, sizeof(*ft));
	ft->len = 0;
	ft->table = NULL;
	memset(ft->lookup, 0, FAPTIME_LOOKUP_TABLE_SIZE);
	return ft;
}

/* Create a random character table */
int faptime_random_table_string(int size, char *dst) {
	srandom(time(NULL));
	char *table = malloc(sizeof(FAPTIME_AVAILABLE_CHARACTERS)+1);
	strcpy(table, FAPTIME_AVAILABLE_CHARACTERS);

	int max = strlen(table);
	if (size > max || size < 1) {
		return -1;
	}

	int ct = 0, i = 0;
	while (ct < size) {
		do {
			/* Use 'random' rather than 'rand' because all bits in 'random' are usable. */
			i = random() % size;
		} while (! table[i]);
		dst[ct++] = table[i];
		table[i] = '\0';
	}
	dst[size] = '\0';
	assert(size == strlen(dst));
	free(table);
	return size;
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
	ft = NULL;
	return;
}

struct faptime_table *faptime_random_table(int base) {
	int max_base = strlen(FAPTIME_AVAILABLE_CHARACTERS);
	if (base < 2 || base > max_base) {
		return NULL;
	}

	char *dst = calloc(base + 1, sizeof(*dst));
	if (!dst) {
		return NULL;
	}
	memset(dst, 'A', base);
	dst[base] = '\0';
	if (faptime_random_table_string(base, dst) != base) {
		return NULL;
	}

	struct faptime_table *ft = faptime_table_from_string(dst, base, FAPTIME_AVAILABLE_CHARACTERS);
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

int faptime_read_table(char *read_path, size_t maxlen, char *dst) {
	int filedes = open(read_path, O_RDONLY | O_NONBLOCK);
	if (filedes < 0) {
		return -1;
	}

	struct stat st;
	size_t bufsize = 0;
	if (0 == fstat(filedes, &st)) {
		/* In a normal case, st.st_size, a map_t (long long on my machine), is
		   promoted to unsigned long long. This should be fine because a file's
		   size cannot be negative. If map_t is larger than size_t, size_t
		   should be promoted, and the comparison would succeed as expected. */
		if (st.st_size - SIZE_MAX > 0) {
			goto cleanup;
		}
		bufsize = st.st_size - maxlen > 0 ? maxlen : st.st_size;
	} else {
		goto cleanup;
	}

	char *tmp = calloc(bufsize+1, sizeof(char));
	if (! tmp) {
		goto cleanup;
	}

	if (read(filedes, tmp, bufsize) < 1) {
		free(tmp);
		goto cleanup;
	}

	tmp[bufsize] = '\0'; 		/* Probably redundant, since calloc should have filled with \0 */
	size_t badidx = strspn(tmp, FAPTIME_AVAILABLE_CHARACTERS);
	memset(dst, '\0', maxlen);

	/* Copy until first invalid character */
	memcpy(dst, tmp, badidx);
	free(tmp);
	close_filedes(filedes);
	return badidx;

 cleanup:
	close_filedes(filedes);
	return -1;
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

int faptime_valid_table(char *table, size_t table_len, char* allowed) {
	unsigned char seen[128] = { 0 };
	unsigned char c;
	/* Ensure no illegal characters */
	if (strspn(table, allowed) < table_len) {
		return 0;
	}

	for (size_t i = 0; i < table_len; i++) {
		c = table[i];
		/* Greater than 127 check so table doesn't have to fit all unsigned chars */
		if (c > 127 || seen[c]) {
			return 0;
		}
		seen[c] = 1;
	}
	return 1;
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



struct faptime_table * faptime_table_from_string(char *table, size_t table_len, char* allowed) {
	if (! faptime_valid_table(table, table_len, allowed)) {
		return NULL;
	}

	struct faptime_table *ft = faptime_alloc_table();
	if (! ft) {
		return NULL;
	}

	ft->len = table_len;
	char *copy = calloc(table_len+1, sizeof(*copy));
	if (! copy) {
		faptime_free_table(ft);
		return NULL;
	}

	strncpy(copy, table, table_len);
	ft->table = copy;

	faptime_init_lookup_table(ft);
	return ft;
}

struct faptime_table *faptime_table_from_file(char *path) {
	size_t len = strlen(FAPTIME_AVAILABLE_CHARACTERS);
	char *table = calloc(len + 1, sizeof(*table));
	if (!table) {
		return NULL;
	}

	faptime_read_table(path, len, table);
	if (strnlen(table, len+1) > len) {
		exit(FAPTIME_ERR_BAD_TABLE);
	}

	struct faptime_table *ft = faptime_table_from_string(table, strnlen(table, len),
														 FAPTIME_AVAILABLE_CHARACTERS);
	free(table);
	if (!ft) {
		return NULL;
	}
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

static struct faptime_table *global_table = NULL;
struct faptime_table *faptime_get_global_table(void) {
	return global_table;
}

int faptime_set_global_table(struct faptime_table *ft) {
	faptime_free_table(global_table);
	global_table = faptime_table_copy(ft);
	return global_table ? 1 : 0;
}
