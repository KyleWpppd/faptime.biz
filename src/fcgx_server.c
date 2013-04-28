/**
 * fcgx_server.c
 *
 * Faptime.biz
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#include "fcgi_config.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "hiredis.h"

#include "encode.h"
#include "faptime_redis.h"
#include "faptime_utils.h"

#include "fcgiapp.h"

#define ERR_NO_SOCK 1

int default_message(FCGX_Stream *out, FCGX_ParamArray *envp, int *count) {
	int i;
	i = FCGX_FPrintF(out,
			 "Content-type: text/html\r\n"
			 "\r\n"
			 "<title>FastCGI echo (fcgiapp version)</title>\n"
			 "<h1>FastCGI echo (fcgiapp version)</h1>\n"
			 "Request number %d,	Process ID: %d<p>\n"
			 "Requested url '%s'\n",
			 ++(*count), getpid(),
			 FCGX_GetParam("REQUEST_URI", *envp));
	return i;
}

int main()
{
	FCGX_Stream *in, *out, *err;
	FCGX_ParamArray envp;
	redisContext *rc = faptime_redis_connect();
	if (NULL == rc) {
		error_log("Unable to connect to Redis");
		/* exit(1); */
	}

	faptime_create_lookup_table();

	int count = 0;
	int redis_errno = 0;
	long long id = 0;
	char *redirect_to, *req_uri;
	while (FCGX_Accept(&in, &out, &err, &envp) >= 0) {
		FAPTIME_REQ_INIT();
		req_uri = FCGX_GetParam("REQUEST_URI", envp);
		error_log("This is an error mofo");
		debug_log("== Begin Request ==");

		/* default_message(out, &envp, &count); */
		/* continue; */

		if (req_uri == NULL) {
			error_log("request_uri was NULL");
			faptime_set_status(500);
			FAPTIME_REQ_FINISH();
			continue;
		}

		if (valid_hash(req_uri) != FAPTIME_HASH_OK || !(id = faptime_decode(req_uri))) {
			debug_log("Request URI '%s' was not valid", req_uri);
			faptime_set_status(404);
			FAPTIME_REQ_FINISH();
			continue;

		}

		if ((redirect_to = faptime_get_url(rc, id, &redis_errno)) == NULL) {
			assert(redirect_to == NULL);
			debug_log("Request hash %lld (%s) had no url", id, req_uri);
			faptime_set_status(404);
			FAPTIME_REQ_FINISH();
			continue;

		}

		faptime_set_status(302);
		faptime_set_redirect(redirect_to);
		free(redirect_to);
		FAPTIME_REQ_FINISH();
	}
	return 0;
}
