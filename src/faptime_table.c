#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
extern int errno;

#include "faptime_table.h"

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

/* Create a random character table */
int faptime_random_table(int size, char *dst) {
	srandom(time(NULL));
	char *table = FAPTIME_AVAILABLE_CHARACTERS;
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
		dst[ct] = table[i];
		table[i] = '\0';
		ct++;
	}
	dst[size - 1] = '\0';
	return size;
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

char * faptime_read_table(char *read_path, size_t maxlen, size_t *size_out) {
	int filedes = open(read_path, O_RDONLY | O_NONBLOCK);
	if (filedes < 1) {
		return NULL;
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

	char* dst = calloc(bufsize, sizeof(char));
	if (!dst) {
		goto cleanup;
	}

	size_t res = read(filedes, dst, bufsize - 1);
	if (res < 1) {
		free(dst);
		goto cleanup;
	}
	*size_out = res;
	return dst;

 cleanup:
	close_filedes(filedes);
	return NULL;
}

int faptime_print_table(struct faptime_table *ft) {
	return fprintf(stderr, "Table:\n%s\n", ft->table);
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

static struct faptime_table * faptime_alloc_table() {
	struct faptime_table *ft = calloc(1, sizeof(struct faptime_table));

	return ft;
}

static void faptime_init_lookup_table(char *table, size_t table_len, struct faptime_table *ft) {
	for (size_t i = 0; i < FAPTIME_LOOKUP_TABLE_SIZE; i++) {
		ft->lookup[i] = -1;
	}

	for (size_t i = 0; i < table_len; i++) {
		unsigned char c = table[i];
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
	char *copy = calloc(table_len+1, sizeof(*table));
	if (! copy) {
		free(ft);
		return NULL;
	}

	ft->table = copy;

	faptime_init_lookup_table(table, table_len, ft);

}
