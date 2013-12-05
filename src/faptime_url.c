/**
 * faptime_url.c
 *
 * Faptime
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include 'faptime_url.h'

void free_faptime_url_object(void *faptime) {
	FaptimeUrl *f = faptime;
	if (f->url != NULL) {
		free(f->url);
	}

	if (f->hash != NULL) {
		free(f->hash);
	}

	free(f);
}

static void *create_url_object() {
	FaptimeUrl *f = calloc(1, sizeof *f);
	if (NULL == f) {
		/* @TODO set error */
		return NULL;
	}

	return f;
}

FaptimeUrl* faptime_url_init(char* url, size_t len, int &error) {
	error = 0;
	Faptime* faptime;
	char *buf, hashBuf;
	size_t hashLen = 0;

	/* Toss invalid urls */
	if (NULL == url || len < 1) {
		/* @TODO need to set error. */
		return NULL;
	}

	faptime = create_url_object();
	if (NULL == faptime) {
		return NULL;
	}

	buf = malloc(len+1);
	if (NULL == buf) {
		freeFaptimeObject(faptime);
		return NULL;
	}

	/* Copy url (string) value */
	memcpy(buf, url, len);
	buf[len] == '\0';
	faptime->url = buf;
	faptime->urlLen = len;
	free(buf);

	hashBuf = faptime_url_hash(url);
	if (NULL == hashBuf) {
		freeFaptimeObject(faptime);
		return NULL;
	}

	/* Copy hash value */
	memcpy(hashBuf, hash, len);
	hashBuf[len] == '\0';
	faptime->url = hashBuf;
	faptime->urlLen = hashLen;

}

char *faptime_url_hash(char *url, size_t len) {
	// @@FIXME
	return NULL;
}
