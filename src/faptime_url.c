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
	if (err) *err = 0;
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
	if (!faptime_url_hash(faptime, url, len, err)) {
		free_faptime_url_object(faptime);
	}
	/* if (NULL == hashBuf) { */
	/* 	free_faptime_url_object(faptime); */
	/* 	return NULL; */
	/* } */

	/* faptime->hash = hashBuf; */
	/* faptime->hashLen = CC_SHA256_DIGEST_LENGTH; */

	return faptime;
}

int faptime_url_hash(FaptimeUrl *f, char *url, size_t len, int *err) {
	if (err) *err = 0;
	unsigned char *digest = calloc(1 + CC_SHA256_DIGEST_LENGTH, sizeof *digest);
	if (NULL == digest) {
		return 0;
	}


	unsigned char *ret = CC_SHA256(url, len, digest);
	assert(ret == digest);
	digest[CC_SHA256_DIGEST_LENGTH] = '\0';

	/* And the human-readable version */
	/* It's double length because we cannot display the high bit. */
	int hash_len = CC_SHA256_DIGEST_LENGTH * 2;
	char *readable_hash = calloc(1 + hash_len, sizeof *readable_hash);
	if (NULL == readable_hash) {
		free(digest);
		return 0;
	}

	char rep[3];
	int offset = 0;
	for (int i = 0; i < CC_SHA256_DIGEST_LENGTH; ++i) {
		/* snprintf(&readable_hash[i*2], sizeof(readable_hash[i*2]), "%02x", digest[i]); */
		snprintf(rep, 3*sizeof(char), "%02x", digest[i]);
		printf("Hashed char: %s\tfor: %ul\n", rep, digest[i]);
		readable_hash[offset] = rep[0];
		readable_hash[offset+1] = rep[1];
		offset = offset + 2;
	}

	printf("Readable hash: %s\n", readable_hash);

	f->digest = digest;
	f->digestLen = CC_SHA256_DIGEST_LENGTH;
	f->hash = readable_hash;
	f->hashLen = hash_len;

	return 1;
}


int main (int argc, char *argv[]) {
	if (argc != 2) return 1;

	for(int i = 0; i < argc; i++) {
		printf("arg %d: %s\n", i, argv[i]);
	}

	printf("URL: %s\n", argv[1]);
	char *url = argv[1];
	/* char *url = "my first url"; */
	int len = strlen(url);
	int err = 6;
	printf("ERR: %d\n", err);
	FaptimeUrl *f = faptime_url_init(url, len, &err);
	printf("ERR2: %d\n", err);
	printf("URL: %s\n", f->url);
	printf("HASH: %s\n", f->hash);
	printf("HASH LEN: %d\n", f->hashLen);
	return 0;
}
