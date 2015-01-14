#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "faptime_utils.h"

#include "faptime_header.h"

#define H_BUF_LEN 5000

static char *h_buf = NULL;
static int req_status = 0;


int faptime_header_init()
{
	req_status = 200;
	faptime_header_free();
	debug_log("Allocating header buffer '%p'", h_buf);
	if (NULL == (h_buf = malloc(sizeof(char) * H_BUF_LEN+1))) {
		error_log("Failed to allocate header buffer");
		return 0;
	}
	debug_log("Allocated header buffer @ '%p'", h_buf);
	/** @FIXME this is a hack because somewhere we aren't null-terminating
	 * properly. This means headers are added on each input. This fixes the
	 * issue, but I don't know why it's an issue to begin with.
	 */
	memset(h_buf, '\0', H_BUF_LEN * sizeof(h_buf[0]));
	return 1;
}

int faptime_set_status(int status)
{
	int tmp = req_status;
	req_status = status;
	FCGX_SetExitStatus(status,
			   faptime_get_stream(FAPTIME_STREAM_OUT));
	debug_log("Setting req status to %d", status);
	return tmp;
}


char *faptime_status_header(int code)
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

	char *header;
	/* WARNING: This differs from CGI */
	if (asprintf(&header, "Status: %s\r\n"
		     "Content-Type: text/html\r\n", status) < 0 ) {
		return NULL;
	}
	return header;
}

int faptime_header_print()
{
	int c = 0;
	FCGX_Stream *out = faptime_get_stream(FAPTIME_STREAM_OUT);
	/* If this is NULL what do we do? It's our status! */
	char *status = faptime_status_header(req_status);
	if (status == NULL) {
		error_log("Unable to allocate memory for status header. "
			  "This is probably fatal");
		exit(1);
	}

	c += FCGX_FPrintF(out, status);
	c += FCGX_FPrintF(out, h_buf);
	c += FCGX_FPrintF(out, "\r\n");
	debug_log("Trying to print headers:\n**%s%s\r\n", status, h_buf);
	free(status);
	return c;
}

int faptime_header_free()
{
	debug_log("Freeing header buffer %p", h_buf);
	free(h_buf);
	return 1;
}

int faptime_header_add(char *header)
{
	return (faptime_header_cat(header) >= 0);
}

int faptime_header_cat(char *header) {
	/* @TODO: Check to see if header was \r\n terminated */
	if (h_buf == NULL && !faptime_header_init()) {
		error_log("Unexpected! h_buf was NULL");
		exit(1);
	}

	size_t remain = H_BUF_LEN - strlen(h_buf);
	if (strlen(header)+strlen("\r\n")+1 > remain) {
		error_log("Header buffer exhausted.\n");
		return -1;
	}

	strncat(h_buf, header, remain-3);
	strncat(h_buf, "\r\n\0", 3);

	debug_log("HBUF now:\n%s", h_buf);

	return H_BUF_LEN - strlen(h_buf);
}

int faptime_set_redirect(char *url)
{
	char *s = NULL;
	if (asprintf(&s, "Location: %s", url) < 0 || s == NULL) {
		error_log("Unable to allocate memory for redirect");
		return 0;
	}
	faptime_header_cat(s);
	free(s);
	return 1;
}
