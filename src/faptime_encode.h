/**
 * faptime_encode.h
 *
 * Faptime
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#ifndef HAVE_FAPTIME_ENCODE_H
#define HAVE_FAPTIME_ENCODE_H

#define FAPTIME_HASH_OK 0
#define FAPTIME_HASH_EMPTY 1
#define FAPTIME_HASH_NOT_PATH 2
#define FAPTIME_HASH_BAD_CHAR 3
#define FAPTIME_HASH_BAD_FORMAT 4

void faptime_create_lookup_table();
int valid_hash(const char *hash);
char *faptime_encode(char *dest, long long num);
long long faptime_decode(char *hash);

#endif
