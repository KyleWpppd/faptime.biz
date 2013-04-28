/**
 * server.h
 *
 * Faptime
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#ifndef HAVE_FAPTIME_SERVER_H
#define HAVE_FAPTIME_SERVER_H

#define FAPTIME_NOT_FOUND 404
#define FAPTIME_REDIRECT_OK 302
#define FAPTIME_SERVER_ERROR 500

//extern int main(void);
extern int status_header(int code);

extern int faptime_redirect(redisContext *context, long long url_id);
extern int _redirect_header(const char *loc, int len);
extern int not_found(void);
extern int server_error(void);
extern int method_not_allowed(const char *allow);

#endif
