/**
 * server.h
 *
 * Faptime.biz
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#ifndef HAVE_FAPTIME_H
#define HAVE_FAPTIME_H

extern int main(void);
extern void parse_response(redisReply *);

extern int is_valid_hash(const char *hash);
extern char *faptime_encode(char *dest, long long num);
extern long long faptime_decode(char *hash);

extern int status_header(int code);
extern int do_redirect(const char *loc, int len);
extern int not_found();
extern int method_not_allowed(const char *allow);

#endif
