/**
 * server.c
 *
 * Faptime.biz
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#include "fcgi_stdio.h"	    /* Must include before stdio.h */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <curl/curl.h>

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
	/*		0	1	2	3	4	5	6	7	8	9 */
        /*  0 */	'W',	'n',	'l',	'V',	'9',	'k',	'u',	'B',	'F',	'4',
        /* 10 */	'a',	'X',	'x',	'Y',	'c',	'R',	'o',	'f',	'A',	'Q',
        /* 20 */	'S',	'3',	'I',	'm',	'T',	'p',	'H',	'7',	'P',	'b',
        /* 30 */	'0',	'N',	'G',	'w',	'L',	'Z',	'e',	'g',	'K',	'1',
        /* 40 */	'2',	'v',	'r',	'6',	'O',	'i',	'd',	'5',	'E',	'D',
        /* 50 */	'z',	'J',	's',	'y',	'q',	't',	'U',	'8',	'h',	'C',
        /* 60 */	'M',	'j'
};


char * s_getenv(char * var) {
        char * tmp;
        tmp = getenv(var);
        return tmp == NULL ? "" : tmp;
}


int main() {
	redisContext *context;
	redisReply *reply;

	struct timeval timeout = { 1, 500000 }; // 1.5 seconds
	context = redisConnectWithTimeout(REDIS_SERVER, REDIS_PORT, timeout);
	if (context == NULL || context->err) {
                if (context) {
                        fprintf(stderr, "Connection error: %s\n", context->errstr);
                        redisFree(context);
                } else {
                        fprintf(stderr, "Connection error: can't allocate redis context\n");
                }
                exit(1);
	}

	char hashout[MAX_HASH_LENGTH+1] = {'\0'};
	char request_hash[MAX_HASH_LENGTH+1] = {'\0'};

	int urilen;
        long long url_id = 0;
	while (FCGI_Accept() >= 0) {
                /* Only accept GET requests */
                if (strcasecmp(s_getenv("REQUEST_METHOD"), "GET") != 0) {
                        method_not_allowed("GET");
                        break;
                }

                // Make sure the hash is not too long
                // No reason to read beyond the max length
                urilen = strnlen(s_getenv("REQUEST_URI"), MAX_HASH_LENGTH+1);
                if (!urilen || urilen > MAX_HASH_LENGTH) {
                        not_found();
                        fprintf(stderr, "URI was invalid: '%s'\n", s_getenv("REQUEST_URI"));
                        break;
                }

                strcpy(request_hash, s_getenv("REQUEST_URI"));
                if (is_valid_hash(request_hash) == HASH_OK) {
                        url_id = faptime_decode(request_hash);
                        fprintf(stderr, "Hash maps to id: %lld\n", url_id);
                        reply = redisCommand(context,"GET urlkey:%lld", url_id);
                        freeReplyObject(reply);
                } else {
                        not_found();
                        fprintf(stderr, "Hash was invalid.\n");
                        break;

                }

                reply = redisCommand(context,"GET urlkey:%s", request_hash);
                printf(
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/plain\r\n"
                        "\r\n"
                );

                fprintf(stderr, "Hash: %s\n", faptime_encode(hashout, atoll(s_getenv("ENCODE"))));

                fprintf(stderr, "PING: %s\n", reply->str);
	}
	redisFree(context);

	return 0;
}

int not_found() {
	return status_header(404);
}


int method_not_allowed(const char *allow) {
        int len = status_header(405) +
                printf("Allow: %s\r\n", allow);
        return len;
}


int is_valid_hash(const char *hash) {
	int len = strnlen(hash, MAX_HASH_LENGTH+1);
	// 0-length string
	if (len == 0) return HASH_EMPTY;
	if (len > MAX_HASH_LENGTH) return HASH_BAD_FORMAT;
	if (hash[0] != '/') {
                fprintf(stderr, "No leading slash\n");
                return HASH_NOT_PATH;
        }
	// Else check each char
	int i = 0;
	while (hash[++i] != '\0' && i <= MAX_HASH_LENGTH) {
		fprintf(stderr, "Alnum @ %d?	 '%c' \n", i, hash[i]);
                if (! isalnum(hash[i])) return HASH_BAD_CHAR;
	}
	fprintf(stderr, "Hash OK!\n");
	return HASH_OK;
}


char * faptime_encode(char *dest, long long num) {
	strcpy(dest, "");
	char * tmp = dest;
	int remainder;
	char c;
	fprintf(stderr, "Now encoding: %lld\n", num);
	while (num != 0) {
                remainder = num % TABLE_SIZE;
                num = num / TABLE_SIZE;
                *dest++ = c = table[remainder];
                fprintf(stderr, "Chr: '%c'\tNum: %lld,\tRem: %d\n", c, num, remainder);
	}
	*dest = '\0';
	return tmp;
}

long long faptime_decode(char *hash) {
	if (strnlen(hash, MAX_HASH_LENGTH+1) > MAX_HASH_LENGTH || ! strlen(hash)) {
                return -1;
	}

	long long num = 0;
	int len = strlen(hash);
	for (int i = 1; i <= len; i += 1) {
                num += hash[len-i] * i;
	}

	return num;
}

int status_header(int code)
{
	char status[128] = {'\0'};
	switch (code) {
                // 200 level
	case 200: strcpy(status, "200 OK"); break;
	case 201: strcpy(status, "201 Created"); break;

                // 300 level
	case 301: strcpy(status, "301 Moved Permanently"); break;
	case 302: strcpy(status, "302 Found"); break;

                // 400 level
	case 400: strcpy(status, "400 Bad Request"); break;
	case 404: strcpy(status, "404 Not Found"); break;
	case 405: strcpy(status, "405 Method Not Allowed"); break;
	case 422: strcpy(status, "422 Unprocessable Entity"); break;
	case 429: strcpy(status, "429 Too Many Requests"); break;

	case 500:
	default:
                strcpy(status, "500 Internal Server Error"); break;
	}

	return printf("HTTP/1.1 %s\r\n", status);
}

int do_redirect(const char *loc, int len) {
	char *encoded;
	int success;
	if ( (encoded = curl_easy_escape(NULL, loc, len)) != NULL) {
                status_header(302);
                printf("Location: %s", encoded);
                curl_free(encoded);
                success = 0;
	} else {
                status_header(500);
                success = -1;
	}

	return success;
}

void parse_response(redisReply *r)
{
	switch (r->type) {
	case REDIS_REPLY_NIL:
		not_found();
		break;
	case REDIS_REPLY_STRING:
		do_redirect(r->str, r->len);
		break;
	case REDIS_REPLY_ERROR:
	default:
		status_header(500);
		break;
	}
}

