/**
 *Faptime.biz
 * A url shortener
 * Copyright 2013, Kyle Wanamaker
 */

#define REDIS_PORT 6379
#define REDIS_SERVER (char*)"127.0.0.1"

/* #include "fcgi_stdio.h" */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hiredis.h"

int main(void) {
    redisContext *c;
    redisReply *reply;

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    c = redisConnectWithTimeout(REDIS_SERVER, REDIS_PORT, timeout);
    if (c == NULL || c->err) {
        if (c) {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
    }
    /* PING server */
    reply = redisCommand(c,"PING");
    printf("PING: %s\n", reply->str);
    freeReplyObject(reply);

    return 0;
}
