# Faptime.biz Makefile
# Copyright (c) 2013 Kyle Wanamaker <kyle dot wanamaker at gmail dot com>
# This file is licensed under the GNU GPLv3

# Fallback to gcc when $CC is not in $PATH.
# Credit to Hiredis github.com/redis/hiredis
CC:=$(shell sh -c 'type $(CC) >/dev/null 2>/dev/null && echo $(CC) || echo gcc')

all:
	$(CC) -o faptime -I/usr/local/include/hiredis -lhiredis src/server.c

clean:
	rm faptime
