#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "faptime_utils.h"

#include "faptime_header.h"

#define HEADER_BUFFER_LENGTH 5000

char *h_buf;
int request_id;

int faptime_header_init() {
	faptime_header_free();
	if (NULL == (h_buf = malloc(HEADER_BUFFER_LENGTH+1))) {
		error_log("Failed to allocate header buffer");
		return 0;
	}
	strncpy(h_buf, '\0', sizeof(*h_buf));
	return 1;
}

int faptime_header_free() {
	free(h_buf);
	return 1;
}

int faptime_header_add(int rid, char *header) {
	if (rid != request_id) {
		if (!faptime_header_init())
			return 0;
		request_id = rid;
	}

	return (faptime_header_cat(header) >= 0);
}

int faptime_header_cat(char *header) {
	/* @TODO: Check to see if header was \r\n terminated */
	int b_size = sizeof(h_buf);
	if (strlen(header)+strlen("\r\n")+1 > b_size - strlen(h_buf)) {
		error_log("Header buffer exhausted.\n");
		return -1;
	}

	strcat(h_buf, header);
	strcat(h_buf, "\r\n");
	return b_size - strlen(h_buf);
}
