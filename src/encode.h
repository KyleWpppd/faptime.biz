/**
 * encode.h
 *
 * Faptime
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#ifndef HAVE_FAPTIME_ENCODE_H
#define HAVE_FAPTIME_ENCODE_H

extern int invalid_hash(const char *hash);
extern char *faptime_encode(char *dest, long long num);
extern long long faptime_decode(char *hash);

#endif
