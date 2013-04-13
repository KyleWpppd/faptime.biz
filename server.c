/**
 *Faptime.biz
 * A url shortener
 * Copyright 2013, Kyle Wanamaker
 */

#include "fcgi_stdio.h"		/* Must include before stdio.h */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "hiredis.h"
#include "server.h"


#define REDIS_PORT 6379
#define REDIS_SERVER (char*)"127.0.0.1"
#define MAX_HASH_LENGTH 10

#define HASH_OK 0
#define HASH_EMPTY 1
#define HASH_NOT_PATH 2
#define HASH_BAD_CHAR 3
#define HASH_BAD_FORMAT 4

#define TABLE_SIZE 62
const char table[62] = {
	/* WnlV9kuBF4aXxYcRofAQS3ImTpH7Pb0NGwLZegK12vr6Oid5EDzJsyqtU8hCMj */
	/*			0		1		2		3		4		5		6		7		8		9 */
/*	0 */	'W',	'n',	'l',	'V',	'9',	'k',	'u',	'B',	'F',	'4',
/* 10 */	'a',	'X',	'x',	'Y',	'c',	'R',	'o',	'f',	'A',	'Q',
/* 20 */	'S',	'3',	'I',	'm',	'T',	'p',	'H',	'7',	'P',	'b',
/* 30 */	'0',	'N',	'G',	'w',	'L',	'Z',	'e',	'g',	'K',	'1',
/* 40 */	'2',	'v',	'r',	'6',	'O',	'i',	'd',	'5',	'E',	'D',
/* 50 */	'z',	'J',	's',	'y',	'q',	't',	'U',	'8',	'h',	'C',
/* 60 */	'M',	'j'

	/* '\x57',	'\x6E',	'\x6C',	'\x56',	'\x39',	'\x6B',	'\x75',	'\x42',	'\x46',	'\x34', */
	/* '\x61',	'\x58',	'\x78',	'\x59',	'\x63',	'\x52',	'\x6F',	'\x66',	'\x41',	'\x51', */
	/* '\x53',	'\x33',	'\x49',	'\x6D',	'\x54',	'\x70',	'\x48',	'\x37',	'\x50',	'\x62', */
	/* '\x30',	'\x4E',	'\x47',	'\x77',	'\x4C',	'\x5A',	'\x65',	'\x67',	'\x4B',	'\x31', */
	/* '\x32',	'\x76',	'\x72',	'\x36',	'\x4F',	'\x69',	'\x64',	'\x35',	'\x45',	'\x44', */
	/* '\x7A',	'\x4A',	'\x73',	'\x79',	'\x71',	'\x74',	'\x55',	'\x38',	'\x68',	'\x43', */
	/* '\x4D',	'\x6A' */
};

int main(void) {
	redisContext *context;
	redisReply *reply;

	/* char *http_host; */
	char request_hash[MAX_HASH_LENGTH+1];

	struct timeval timeout = { 1, 500000 }; // 1.5 seconds
	context = redisConnectWithTimeout(REDIS_SERVER, REDIS_PORT, timeout);
	if (context == NULL || context->err) {
		if (context) {
			printf("Connection error: %s\n", context->errstr);
			redisFree(context);
		} else {
			printf("Connection error: can't allocate redis context\n");
		}
		exit(1);
	}

	const char *get = "GET";
	const int maxlen = sizeof(request_hash);
	char hashout[MAX_HASH_LENGTH+1];
	int urilen;
	while (FCGI_Accept() >= 0) {
		/* Only accept GET requests */
 		if (strcasecmp(getenv("REQUEST_METHOD"), get) != 0) {
			method_not_allowed(get);
			break;
		}

		// Make sure the hash is not too long
		urilen = strnlen(getenv("REQUEST_URI"), maxlen);
		if (!urilen || urilen > maxlen) {
			not_found();
            printf("URI was invalid: '%s'\n", getenv("REQUEST_URI"));
			break;
		}

		strcpy(request_hash, getenv("REQUEST_URI"));
		if (is_valid_hash(request_hash) != HASH_OK) {
			not_found();
            printf("Hash was invalid.\n");
			break;
		}

		reply = redisCommand(context,"GET urlkey:%s", request_hash);
		printf(
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"\r\n"
		);

        printf("Hash: %s\n", faptime_encode(hashout, atoll(getenv("ENCODE"))));

		printf("PING: %s\n", reply->str);
		freeReplyObject(reply);
	}
	redisFree(context);

	return 0;
}

int not_found() {
	return printf(
		"HTTP/1.1 404 Not Found\r\n"
		"Server: faptime.biz\r\n"
		"Content-Length: 0\r\n"
		"\r\n"
	);
}

int method_not_allowed(const char *allow) {
	return printf(
		"HTTP/1.1 405 Method Not Allowed\r\n"
		"Allow: %s\r\n"
		"Content-Length: 0\r\n"
		"\r\n", allow
	);
}


int is_valid_hash(const char *hash) {
	int i = 0;
    int len = strnlen(hash, MAX_HASH_LENGTH+1);
	// 0-length string
	if (len == 0) return HASH_EMPTY;
    if (len > MAX_HASH_LENGTH) return HASH_BAD_FORMAT;
	if (hash[i] != '/') return HASH_NOT_PATH;
	// Else check each char
    i++;
	while (hash[i] != '\0' && i <= MAX_HASH_LENGTH) {
        printf("Alnum ?  '%c' \n", hash[i]);
		if (! isalnum(hash[i])) return HASH_BAD_CHAR;
		i++;
	}
	printf("Hash OK!\n");
	return HASH_OK;
}


char * faptime_encode(char * dest, long long num) {
	strcpy(dest, "");
    char * tmp = dest;
	int remainder;
    char c;
    printf("Now encoding: %d\n", num);
	while (num != 0) {
		remainder = num % TABLE_SIZE;
		num = num / TABLE_SIZE;
		*dest++ = c = table[remainder];
        printf("Chr: '%c'\tNum: %d,\tRem: %d\n", c, num, remainder);
	}
    *dest = '\0';
	return tmp;
}
