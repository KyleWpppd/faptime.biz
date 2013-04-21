/**
 * encode.c
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

#include "encode.h"

#define HASH_OK 0
#define HASH_EMPTY 1
#define HASH_NOT_PATH 2
#define HASH_BAD_CHAR 3
#define HASH_BAD_FORMAT 4

#define MAX_HASH_LENGTH 10

#define TABLE_SIZE 62
const char table[62] = {
	/* WnlV9kuBF4aXxYcRofAQS3ImTpH7Pb0NGwLZegK12vr6Oid5EDzJsyqtU8hCMj */
	/*        0    1    2    3    4    5    6    7    8    9 */
	/*  0 */ 'W', 'n', 'l', 'V', '9', 'k', 'u', 'B', 'F', '4',
	/* 10 */ 'a', 'X', 'x', 'Y', 'c', 'R', 'o', 'f', 'A', 'Q',
	/* 20 */ 'S', '3', 'I', 'm', 'T', 'p', 'H', '7', 'P', 'b',
	/* 30 */ '0', 'N', 'G', 'w', 'L', 'Z', 'e', 'g', 'K', '1',
	/* 40 */ '2', 'v', 'r', '6', 'O', 'i', 'd', '5', 'E', 'D',
	/* 50 */ 'z', 'J', 's', 'y', 'q', 't', 'U', '8', 'h', 'C',
	/* 60 */ 'M', 'j'
};

int invalid_hash(const char *hash)
{
	int len = strnlen(hash, MAX_HASH_LENGTH + 1);

	if (len == 0)
		return HASH_EMPTY;
	if (len > MAX_HASH_LENGTH)
		return HASH_BAD_FORMAT;
	if (hash[0] != '/')
		return HASH_NOT_PATH;

	int i = 0;
	while (hash[++i] != '\0' && i <= MAX_HASH_LENGTH) {
		fprintf(stderr, "Alnum @ %d?	 '%c' \n", i, hash[i]);
		if (!isalnum(hash[i]))
			return HASH_BAD_CHAR;
	}
	fprintf(stderr, "Hash OK!\n");
	return HASH_OK;
}

char *faptime_encode(char *dest, long long num)
{
	strcpy(dest, "");
	char *tmp = dest;
	int remainder;
	char c;
	fprintf(stderr, "Now encoding: %lld\n", num);
	while (num != 0) {
		remainder = num % TABLE_SIZE;
		num = num / TABLE_SIZE;
		*dest++ = c = table[remainder];
		fprintf(stderr, "Chr: '%c'\tNum: %lld,\tRem: %d\n", c, num,
			remainder);
	}
	*dest = '\0';
	return tmp;
}

long long faptime_decode(char *hash)
{
	if (strnlen(hash, MAX_HASH_LENGTH + 1) > MAX_HASH_LENGTH ||
	    !strlen(hash))
		return -1;

	long long num = 0;
	int len = strlen(hash);
	for (int i = 1; i <= len; i += 1) {
		num += hash[len - i] * i;
	}

	return num;
}