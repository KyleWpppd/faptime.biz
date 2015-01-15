#ifndef FAPTIME_STORE_H
#define FAPTIME_STORE_H

#include <stdint.h>
#include "faptime_urls.h"

#define REDIS_ENABLED 1
#define PGSQL_ENABLED 1
#define MYSQL_ENABLED 0
#define MEMCACHE_ENABLED 0

enum faptime_store_db {
	FT_DB_INVALID,
	FT_DB_POSTGRESQL,
	FT_DB_REDIS,
	FT_DB_MEMCACHE,
	FT_DB_MSQL,
};

enum faptime_store_socktype{
	FT_SOCKTYPE_INVALID,
	FT_SOCKTYPE_LOCAL,
	FT_SOCKTYPE_INET,
};

enum faptime_store_type {
	FT_STORE_INVALID,
	FT_STORE_PRIMARY,
	FT_STORE_CACHE,
};

struct faptime_store {
	uint16_t port;
	union {
		char *host;
		char *sock;
	};
	char *user;
	char *pass;
	unsigned int timeout;
	enum faptime_store_db db;
	enum faptime_store_socktype socktype;
};

typedef int (*FaptimeApiFunc_hash_get)(struct faptime_url *url);
typedef int (*FaptimeApiFunc_hash_delete)(struct faptime_url *url);
typedef int (*FaptimeApiFunc_hash_store)(struct faptime_url *url);
typedef int (*FaptimeApiFunc_url_store)(struct faptime_url *url);


struct faptime_store_api_call {
	FaptimeApiFunc_hash_get hash_get;
	FaptimeApiFunc_hash_delete hash_delete;
	FaptimeApiFunc_hash_store hash_store;
	FaptimeApiFunc_url_store url_store;
};




struct faptime_store *faptime_store_alloc_store();

int faptime_store_set_store(struct faptime_store *store, enum faptime_store_type type);
int faptime_store_set_primary(struct faptime_store *store);
int faptime_store_set_cache(struct faptime_store *store);
int faptime_store_hash_get(struct faptime_url *furl);
int faptime_store_copy_store(struct faptime_store *dst, const struct faptime_store *src);

int faptime_store_valid_store(struct faptime_store *store);

int faptime_store_init_db(enum faptime_store_type type, struct faptime_store *fstore);
int faptime_store__init_pgsql(struct faptime_store *store);
int faptime_store__init_redis(struct faptime_store *store);
/* int faptime_store__init_mysql(struct faptime_store *fstore); */

int faptime_store__url_has_hash(struct faptime_url *furl);
int faptime_store__get_from_cache(struct faptime_url *furl);
int faptime_store__get_from_primary(struct faptime_url *furl);
int faptime_store__write_to_cache(struct faptime_url *furl);
int faptime_store__delete_from_cache(struct faptime_url *furl);
#endif
