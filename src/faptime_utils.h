/**
 * faptime_utils.h
 *
 * Faptime.biz
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#ifndef HAVE_FAPTIME_UTILS
#define HAVE_FAPTIME_UTILS

#define DEBUG 1

#define FAPTIME_STREAM_IN 0
#define FAPTIME_STREAM_OUT 1
#define FAPTIME_STREAM_ERR 2

#include <stdarg.h>

#include "faptime_header.h"

#include "fcgiapp.h"

int faptime_error(const char *format, ...);
FCGX_Stream *faptime_get_stream(int stream_no);
void faptime_error_bind(FCGX_Stream *in, FCGX_Stream *out,
			      FCGX_Stream *err, FCGX_ParamArray envp);

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define DEBUG_LOC  STRINGIFY(__FILE__ ":" TOSTRING(__LINE__) " in " __FUNCTION__ " ")
#define debug_log(...)				\
	do {						\
		if (DEBUG) {				\
			fap_log("[DEBUG] ",  __VA_ARGS__);	\
		}					\
	} while (0)

#define error_log(...)				\
	do {						\
		fap_log("[ERROR] ", __VA_ARGS__);	\
	} while (0)

#define fap_log(prefix, ...)					\
	do { \
		FCGX_Stream *err = faptime_get_stream(2); \
		FCGX_FPrintF(err, prefix " " DEBUG_LOC __VA_ARGS__ ); \
	} while (0) \


#define FAPTIME_REQ_INIT() \
	do {							\
		faptime_error_bind(in, out, err, envp);	\
		int c = faptime_header_init();			\
		debug_log("header init complete (%d)", c);	\
	} while (0)

#define FAPTIME_REQ_FINISH() \
	do {					\
		faptime_header_print();	\
	} while (0)



#endif
