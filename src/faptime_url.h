/**
 * faptime_url.h
 *
 * Faptime
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#ifndef HAVE_FAPTIME_URL_H
#define HAVE_FAPTIME_URL_H

// I should really do something with the strings, like abstract them
// into their own type with safe handling functions.

typedef struct FaptimeUrl
{
	unsigned long long id;
	int status;
	char* url;
	int urlLen;
	unsigned char *digest;
	int digestLen;
	char* hash;
	int hashLen;
} FaptimeUrl;

// Attempts to insert the url into the DB and returns 0 on failure
// this should be static in the file
// unsigned long long _faptime_url_insert(FaptimeUrl* url, int &error);
unsigned long long faptime_url_save(FaptimeUrl* url, int *err);
FaptimeUrl* faptime_url_init(char* url, size_t len, int *err);
void free_faptime_url_object(void *faptime);
int faptime_url_hash(FaptimeUrl *f, char *url, size_t len, int *err);

#endif
