/**
 * server.c
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

/* #include "hiredis.h" */
/* #include "encode.h" */
/* #include "fcgx_server.h" */
#include "faptime_utils.h"
#include "fcgiapp.h"		/* Must include before stdio.h */

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

	int count = 0;
	while (FCGX_Accept(&in, &out, &err, &envp) >= 0) {
		FAPTIME_REQ_INIT();;
		debug_log("this is a debug message");
		default_message(out, &envp, &count);
	}
	return 0;
}
