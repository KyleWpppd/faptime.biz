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

#include <string.h>
#include <stdarg.h>

#include "fcgiapp.h"

extern int faptime_error(const char *format, ...);
extern FCGX_Stream *faptime_get_stream(int stream_no);
extern int faptime_error_bind(FCGX_Stream *in, FCGX_Stream *out,
			      FCGX_Stream *err, FCGX_ParamArray envp);

#define FAPTIME_REQ_INIT() \
	do {							\
		faptime_error_bind(in, out, err, envp);	\
	} while (0)

#define poop() \
	do {   \
		if (DEBUG);			\
	} while (0)


#define debug_log(fmt, ...)				\
	do {						\
		if (DEBUG) {				\
			error_log(fmt, ##__VA_ARGS__);	\
		}					\
	} while (0)

#define error_log(fmt, ...)						\
	do {								\
		FCGX_Stream *err = faptime_get_stream(2);		\
		FCGX_FPrintF(err, "[ERROR] %s:%d:%s(): " fmt, __FILE__,	\
			     __LINE__, __func__, ##__VA_ARGS__);		\
	} while (0)



#endif
