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
#include <stdio.h>
#include <string.h>

/* @TODO check and see if this needs to be a macro so it can be built on other targets
   CommonCrypto just feels like it's an OS X specific thing. */
#if defined(__APPLE__)
#include <CommonCrypto/CommonDigest.h>
#else
#include <openssl/sha.h>
#endif

#include "faptime_url.h"

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

FaptimeUrl* faptime_url_init(char* url, size_t len, int *err) {
	*err = 0;
	FaptimeUrl *faptime;
	char *buf;
	unsigned char *hashBuf;
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
		free_faptime_url_object(faptime);
		return NULL;
	}

	/* Copy url (string) value */
	memcpy(buf, url, len);
	buf[len] = '\0';
	faptime->url = buf;
	faptime->urlLen = len;
	free(buf);


	/* @TODO, this is wrong because I am allocating based on the
	   implementation of another function. Ideally, the length should be set
	   in the function calling the string.
	 */
	hashBuf = faptime_url_hash(url, len, err);
	if (NULL == hashBuf) {
		free_faptime_url_object(faptime);
		return NULL;
	}

	faptime->hash = hashBuf;
	faptime->hashLen = CC_SHA256_DIGEST_LENGTH;

	return faptime;
}

unsigned char *faptime_url_hash(char *url, size_t len, int *err) {
	*err = 0;
	unsigned char *digest = calloc(1 + CC_SHA256_DIGEST_LENGTH, sizeof(*digest));
	if (NULL == digest) {
		return NULL;
	}


	unsigned char *ret = CC_SHA256(url, len, digest);
	assert(ret == digest);
	digest[CC_SHA256_DIGEST_LENGTH] = '\0';

	/* And the human-readable version */
	/* It's double length because we cannot display the high bit. */
	int hash_len = CC_SHA256_DIGEST_LENGTH * 2;
	char *readable_hash = calloc(1 + hash_len, sizeof(*readable_hash));
	if (NULL == readable_hash) {
		free(digest);
		return NULL;
	}

	for (int i = 0; i < CC_SHA256_DIGEST_LENGTH; ++i) {
		snprintf(&readable_hash[i*2], sizeof(readable_hash[i*2]), "%02x", digest[i]);
	}

	return digest;
}
