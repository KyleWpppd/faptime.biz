/**
 * server.c
 *
 * Faptime.biz
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#include "fcgi_stdio.h"		/* Must include before stdio.h */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <curl/curl.h>

#include "hiredis.h"
#include "encode.h"
#include "server.h"


#define REDIS_PORT 6379
#define REDIS_SERVER (char*)"127.0.0.1"
#define ERR_REDIS_CONNECT 4

char *s_getenv(char *var)
{
	char *tmp;
	tmp = getenv(var);
	return tmp == NULL ? "" : tmp;
}

redisContext *faptime_redis_connect() {
	redisContext *context;
	struct timeval timeout = { 1, 500000 };	// 1.5 seconds

	context =
	    redisConnectWithTimeout(REDIS_SERVER, REDIS_PORT, timeout);
	if (context == NULL) {
		fprintf(stderr,
			"Connection error: can't allocate redis context\n");
		return NULL;
	} else if (context->err) {
		fprintf(stderr, "Connection error: %s\n", context->errstr);
		redisFree(context);
		return NULL;
	}
	return context;
}

int main()
{
	redisContext *context;
	if (NULL == (context = faptime_redis_connect())) {
		exit(ERR_REDIS_CONNECT);
	}

	long long url_id = 0;
	char *req_uri;

	while (FCGI_Accept() >= 0) {
		/* Only accept GET requests */
		if (strcasecmp(s_getenv("REQUEST_METHOD"), "GET") != 0) {
			method_not_allowed("GET");
			break;
		}

		req_uri = s_getenv("REQUEST_URI");
		if (invalid_hash(req_uri)) {
			not_found();
			fprintf(stderr, "Hash was invalid.\n");
			break;
		}

		url_id = faptime_decode(req_uri);
		switch (faptime_redirect(context, url_id)) {
		case FAPTIME_REDIRECT_OK:
			break;
		case FAPTIME_NOT_FOUND:
			not_found();
			break;
		case FAPTIME_SERVER_ERROR:
		default:
			server_error();
			break;
		}
	} /* while */
	redisFree(context);

	return 0;
}

int server_error()
{
	return status_header(500);
}

int not_found()
{
	return status_header(404);
}


int method_not_allowed(const char *allow)
{
	int len = status_header(405) + printf("Allow: %s\r\n", allow);
	return len;
}

int status_header(int code)
{
	char status[128] = { '\0' };
	switch (code) {

	case 200:
		strcpy(status, "200 OK");
		break;
	case 201:
		strcpy(status, "201 Created");
		break;

	case 301:
		strcpy(status, "301 Moved Permanently");
		break;
	case 302:
		strcpy(status, "302 Found");
		break;

	case 400:
		strcpy(status, "400 Bad Request");
		break;
	case 404:
		strcpy(status, "404 Not Found");
		break;
	case 405:
		strcpy(status, "405 Method Not Allowed");
		break;
	case 422:
		strcpy(status, "422 Unprocessable Entity");
		break;
	case 429:
		strcpy(status, "429 Too Many Requests");
		break;

	case 500:
	default:
		strcpy(status, "500 Internal Server Error");
		break;
	}

	return printf("HTTP/1.1 %s\r\n", status);
}

int _redirect_header(const char *loc, int len)
{
	char *encoded;
	int success;
	if ((encoded = curl_easy_escape(NULL, loc, len)) != NULL) {
		status_header(302);
		printf("Location: %s", encoded);
		curl_free(encoded);
		success = 1;
	} else {
		success = 0;
	}

	return success;
}

int faptime_redirect(redisContext *context, long long url_id)
{
	if (url_id < 1)
		return FAPTIME_NOT_FOUND;

	redisReply *reply;
	int status;
	reply = redisCommand(context, "GET urlkey:%lld", url_id);
	switch (reply->type) {
	case REDIS_REPLY_NIL:
		status = FAPTIME_NOT_FOUND;
		break;
	case REDIS_REPLY_STRING:
		status = _redirect_header(reply->str, reply->len) ?
			FAPTIME_REDIRECT_OK : FAPTIME_SERVER_ERROR;
		break;
	case REDIS_REPLY_ERROR:
	default:
		status = FAPTIME_SERVER_ERROR;
		break;
	}

	freeReplyObject(reply);
	return status;
}

