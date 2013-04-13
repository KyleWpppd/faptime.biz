/**
 *Faptime.biz
 * A url shortener
 * Copyright 2013, Kyle Wanamaker
 */

#ifndef HAVE_FAPTIME_H
#define HAVE_FAPTIME_H

extern int main(void);
extern int method_not_allowed(const char *allow);
extern int not_found();
extern int is_valid_hash(const char *hash);
extern char * faptime_encode(char *dest, long long num);
#endif
