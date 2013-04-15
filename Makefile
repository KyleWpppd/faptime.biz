###
# Makefile
#
# Faptime.biz
# A url shortener
# Copyright (c) 2013, Kyle Wanamaker
# Licensed under the GNU GPLv3
# Please see the full text of the license in COPYING
###

# Fallback to gcc when $CC is not in $PATH.
# Credit to Hiredis github.com/redis/hiredis
CFLAGS=-Wall -g -lhiredis  -lfcgi -lcurl
CC:=$(shell sh -c 'type $(CC) >/dev/null 2>/dev/null && echo $(CC) || echo gcc')

all: server

clean:
	rm -r server server.dSYM

