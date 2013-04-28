/**
 * faptime_header.h
 *
 * Faptime
 * A url shortener
 * Copyright (c) 2013, Kyle Wanamaker
 * Licensed under the GNU GPLv3
 * Please see the full text of the license in COPYING
 */

#ifndef HAVE_FAPTIME_HEADER
#define HAVE_FAPTIME_HEADER

int faptime_header_init();
int faptime_header_add(int request_id, char *header);
int faptime_header_cat(char *header);
int faptime_header_free();

#endif
