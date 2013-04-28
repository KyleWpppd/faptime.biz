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
CFLAGS=-Wall -g
LDFLAGS=-lhiredis  -lfcgi -lcurl
CC:=$(shell sh -c 'type $(CC) >/dev/null 2>/dev/null && echo $(CC) || echo gcc')

C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.o)))

#all: encoder server

all: $(OBJ_FILES)

clean:
	-rm -r faptime fcgx obj/

faptime: $(OBJ_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ_FILES)

fcgx: $(OBJ_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ_FILES)

obj/%.o: src/%.c
	-mkdir obj
	$(CC) $(CFLAGS) -c -o $@ $<


