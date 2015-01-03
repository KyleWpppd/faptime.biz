/**
 * faptime_encode.c
 *
 * Faptime
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>

#include "faptime_encode.h"

#include "faptime_utils.h"

#define MAX_HASH_LENGTH 10

#define TABLE_SIZE 62
static const char table[TABLE_SIZE+1] = {
	/* WnlV9kuBF4aXxYcRofAQS3ImTpH7Pb0NGwLZegK12vr6Oid5EDzJsyqtU8hCMj */
	/*        0    1    2    3    4    5    6    7    8    9 */
	/*  0 */ 'W', 'n', 'l', 'V', '9', 'k', 'u', 'B', 'F', '4',
	/* 10 */ 'a', 'X', 'x', 'Y', 'c', 'R', 'o', 'f', 'A', 'Q',
	/* 20 */ 'S', '3', 'I', 'm', 'T', 'p', 'H', '7', 'P', 'b',
	/* 30 */ '0', 'N', 'G', 'w', 'L', 'Z', 'e', 'g', 'K', '1',
	/* 40 */ '2', 'v', 'r', '6', 'O', 'i', 'd', '5', 'E', 'D',
	/* 50 */ 'z', 'J', 's', 'y', 'q', 't', 'U', '8', 'h', 'C',
	/* 60 */ 'M', 'j', '\0'
};

int lookup_table[128] = {0};
void faptime_create_lookup_table() {
	int i = 0;
	char cc;
	for (i = 0; i < strlen(table); i++) {
		cc = table[i];
		assert(isalnum(cc));
		lookup_table[((int) cc)] = i;
	}
}


int valid_hash(const char *hash)
{
	int len = strnlen(hash, MAX_HASH_LENGTH + 1);

	if (len == 0) {
		debug_log("Hash '%s' was empty", hash);
		return FAPTIME_HASH_EMPTY;
	} else if (len > MAX_HASH_LENGTH) {
		debug_log("Hash '%s' was too long", hash);
		return FAPTIME_HASH_BAD_FORMAT;
	} else if (hash[0] != '/') {
		debug_log("Hash '%s' did not have leading slash", hash);
		return FAPTIME_HASH_NOT_PATH;
	}

	int i = 0;
	while (hash[++i] != '\0' && i <= MAX_HASH_LENGTH) {
		debug_log("Alnum @ %d?	 '%c' \n", i, hash[i]);
		if (!isalnum(hash[i]))
			return FAPTIME_HASH_BAD_CHAR;
	}
	debug_log("Hash OK!\n");
	return FAPTIME_HASH_OK;
}

char *faptime_encode(char *dest, long long num)
{
	strcpy(dest, "");
	char *tmp = dest;
	int remainder;
	char c;
	/* debug_log(stderr, "Now encoding: %lld\n", num); */
	while (num != 0) {
		remainder = num % TABLE_SIZE;
		num = num / TABLE_SIZE;
		*dest++ = c = table[remainder];
		debug_log("Chr: '%c'\tNum: %lld,\tRem: %d\n", c, num,
			remainder);
	}
	*dest = '\0';
	return tmp;
}

long long faptime_decode(char *hash)
{
	debug_log("Attempting to decode '%s'", hash);
	debug_log("Lookup table:\n\n%s\n\n", lookup_table);
	if (strnlen(hash, MAX_HASH_LENGTH + 1) > MAX_HASH_LENGTH ||
	    strlen(hash) == 0) {
		debug_log("Hash '%s' was either too long or too short", hash);
		return 0;
	}

	long long num = 0;
	long long n;
	int len = strlen(hash);
	int c;
	for (int i = 1; i <= len; i += 1) {
		c = (int) hash[len - i];
		n = lookup_table[c] * powl(62, i-1);
		debug_log("For '%c', => %d", c, n);
		num += n;
	}
	debug_log("Hash: '%s' => %lld\n", hash, num);
	return num;
}
