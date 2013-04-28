/**
 * faptime_redis.h
 *
 * Faptime
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#ifndef HAVE_FAPTIME_REDIS
#define HAVE_FAPTIME_REDIS

char *faptime_get_url(redisContext *c, long long id, int *err);
redisContext *faptime_redis_connect();

#endif
