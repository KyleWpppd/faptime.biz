#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "faptime_store.h"


struct _faptime_stores {
	struct faptime_store primary;
	struct faptime_store cache;
};

static struct _faptime_stores stores;

static char* copyif(char *src) {
	if (src) {
		char *tmp = strdup(src);
		if (!tmp) {
			fprintf(stderr, "Memory allocation failure in %s\n", __FUNCTION__);
			abort();
		}
		return tmp;
	}
	return NULL;
}

static void _faptime_store_set_defaults(struct faptime_store *fs) {
	fs->port = 0;
	fs->host = NULL;
	fs->sock = NULL;
	fs->user = NULL;
	fs->pass = NULL;

	fs->timeout = 0;
	fs->db = FT_DB_INVALID;
	fs->socktype = FT_SOCKTYPE_INVALID;
}

struct faptime_store *faptime_store_alloc_store(void) {
	struct faptime_store *fs = calloc(1, sizeof(*fs));
	if (!fs) {
		return NULL;
	}
	_faptime_store_set_defaults(fs);
	return fs;
}

int faptime_store_copy_store(struct faptime_store *dst, const struct faptime_store *src) {
	if (src == NULL || dst == NULL) {
		return -1;
	}
	dst->port = src->port;
	dst->host = copyif(src->host);
	dst->user = copyif(src->user);
	dst->pass = copyif(src->pass);
	dst->sock = copyif(src->sock);
	dst->timeout = src->timeout;
	dst->db = src->db;
	dst->socktype = src->socktype;
	return 0;
}

int faptime_store_set_store(struct faptime_store *store, enum faptime_store_type type){
	if (!store || !faptime_store_valid_store(store)) {
		return -1;
	}
	struct faptime_store *dst;
	switch (type) {
	case FT_STORE_PRIMARY:
		dst = &(stores.primary);
		break;
	case FT_STORE_CACHE:
		dst = &(stores.cache);
		break;
	default:
		return -2;
	}

	if (faptime_store_copy_store(dst, store) < 0) {
		return -3;
	}

	return 0;
}

int faptime_store_set_primary(struct faptime_store *store) {
	int rv;
	enum faptime_store_type st = FT_STORE_PRIMARY;
	rv = faptime_store_set_store(store, st);
	return rv;
}

int faptime_store_set_cache(struct faptime_store *store) {
	int rv;
	enum faptime_store_type st = FT_STORE_CACHE;
	rv = faptime_store_set_store(store, st);
	return rv;
}

int faptime_store_valid_store(struct faptime_store *store) {
	if (store->db == FT_STORE_INVALID ||
		store->socktype == FT_SOCKTYPE_INVALID ||
		(! store->host && ! store->sock)) {
		return 0;
	}

	if (store->socktype == FT_SOCKTYPE_LOCAL &&
		strnlen(store->sock, PATH_MAX) > PATH_MAX) {
		return 0;
	} else if (store->socktype == FT_SOCKTYPE_INVALID &&
			   strnlen(store->sock, _POSIX_HOST_NAME_MAX) > _POSIX_HOST_NAME_MAX) {
		return 0;
	}
	return 1;
}

/* FIXME need to work on primary vs caching roles here. */
int faptime_store_init_db(enum faptime_store_db type, struct faptime_store *store) {
	switch (type) {
	case FT_DB_POSTGRESQL:
		return faptime_store__init_pgsql(store);
	case FT_DB_REDIS:
		return faptime_store__init_redis(store);
	default:
		return -1;
	}
}
