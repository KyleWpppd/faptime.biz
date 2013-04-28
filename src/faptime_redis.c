/**
 * faptime_redis.c
 *
 * Faptime
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "faptime_utils.h"
#include "hiredis.h"

#define REDIS_PORT 6379
#define REDIS_SERVER (char*)"127.0.0.1"


redisContext *faptime_redis_connect()
{
	struct timeval timeout = { 1, 500000 };	// 1.5 seconds

	redisContext *context =
	    redisConnectWithTimeout(REDIS_SERVER, REDIS_PORT, timeout);
	if (context == NULL) {
		error_log("Redis connection error: can't allocate redis context");
		return NULL;
	} else if (context->err) {
		error_log("Redis connection error: %s\n", context->errstr);
		redisFree(context);
		return NULL;
	}
	return context;
}


char *faptime_get_url(redisContext *c, long long id, int *err)
{
	if (id < 1) {
		*err = 404;
		return NULL;
	}

	char *url = NULL;
	redisReply *reply;
	reply = redisCommand(c, "GET url_id:%lld", id);

	switch (reply->type) {
	case REDIS_REPLY_STRING:
		*err = 0;
		url = malloc(sizeof(char) * (reply->len+1));
		if (url != NULL)
			(void) strncpy(url, reply->str, reply->len + 1);
		break;

	case REDIS_REPLY_NIL:
		*err = 404;
		return NULL;

	case REDIS_REPLY_ERROR:
	default:
		*err = 500;
		break;
	}

	freeReplyObject(reply);
	return url;
}
