#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <assert.h>

#include "faptime_utils.h"

#include "fcgiapp.h"

static FCGX_Stream *in, *out, *err;
static FCGX_ParamArray envp;

int faptime_error(const char *format, ...)
{
	va_list argptr;
	int len;
	va_start(argptr, format);
	len = FCGX_VFPrintF(err, format, argptr);
	va_end(argptr);
	return len;
}

void faptime_error_bind(FCGX_Stream *_in, FCGX_Stream *_out,
			FCGX_Stream *_err, FCGX_ParamArray _envp)
{
	in = _in;
	out = _out;
	err = _err;
	envp = _envp;
}

/* void fap_log(const char *prefix, const char *fmt, ...) { */
/* { */
/* 	va_list argp; */
/* 	fprintf(stderr, prefix); */
/* 	va_start(argp, fmt); */
/* 	vfprintf(stderr, fmt, argp); */
/* 	va_end(argp); */
/* 	fprintf(stderr, "\n"); */
/* } */

FCGX_Stream *faptime_get_stream(int stream_no)
{
	FCGX_Stream *tmp;
	switch(stream_no) {
	case FAPTIME_STREAM_IN:
		tmp = in;
		break;
	case FAPTIME_STREAM_OUT:
		tmp = out;
		break;
	case FAPTIME_STREAM_ERR:
		tmp = err;
		break;
	default:
		tmp = NULL;
	}
	return tmp;
}
