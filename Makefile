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
CFLAGS=-g -Wall -pedantic -Werror -std=c99 -fno-omit-frame-pointer
LDFLAGS=-lhiredis -lfcgi -lcurl -lcheck -liniparser -v
CC:=$(shell sh -c 'type $(CC) >/dev/null 2>/dev/null && echo $(CC) || echo gcc')

C_FILES := $(wildcard src/*.c)
OBJ_DIR=obj
OBJ_FILES := $(addprefix $(OBJ_DIR)/,$(notdir $(C_FILES:.c=.o)))
NOTMAIN_OBJS := $(filter-out %_main.o, $(OBJ_FILES))

TEST_DIR:=tests
TEST_BINDIR:=$(TEST_DIR)/bin
TEST_FILES:=$(wildcard tests/*.c)
# Create targets for any test_*.c file in the tests/ path
TEST_BINS:=$(patsubst $(TEST_DIR)/test_%.c, $(TEST_DIR)/bin/%-test, $(TEST_FILES))

# Begin to separate out the server from the command line util
FAPTIME_SOURCES := $(wildcard src/faptime_*.c)
FAP_OBJS := $(addprefix $(OBJ_DIR)/,$(notdir $(FAPTIME_SOURCES:.c=.o)))

FCGX_MAIN="fcgx_server_main.o"
FAPTIME_MAIN="faptime_main.o"

.PHONY: all clean test test-bin-folder cppcheck check

faptime: $(FAP_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(FAP_OBJS)

all: faptime fcgx

clean:
	rm -rf faptime fcgx $(OBJ_DIR) $(TEST_BINDIR)


fcgx: $(NOTMAIN_OBJS) $(FCGX_MAIN)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ_FILES) $(FCGX_MAIN)

test: faptime $(TEST_BINS)
	./$<

cppcheck:
	cppcheck --enable=all --inconclusive src

check: test cppcheck

obj/%.o: src/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ $<

test-bin-folder:
	mkdir -p $(TEST_BINDIR)

tests/bin/%.o: tests/%.c test-bin-folder
	$(CC) $(CFLAGS) -c -o $@ $<

%-test: test_%.o $(OBJ_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(NOTMAIN_OBJS) $<
