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
CFLAGS=-Wall -pedantic -Werror -g
LDFLAGS=-lhiredis -lfcgi -lcurl -lcheck
CC:=$(shell sh -c 'type $(CC) >/dev/null 2>/dev/null && echo $(CC) || echo gcc')

C_FILES := $(wildcard src/*.c)
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.c=.o)))

TEST_DIR:=tests
TEST_FILES:=$(wildcard tests/*.c)
# Create targets for any test_*.c file in the tests/ path
TEST_BINS:=$(patsubst $(TEST_DIR)/test_%.c, $(TEST_DIR)/bin/%-test, $(TEST_FILES))

# Begin to separate out the server from the command line util
FAPTIME_SOURCES := $(wildcard src/faptime_*.c)
FAP_OBJS := $(addprefix obj/,$(notdir $(FAPTIME_SOURCES:.c=.o)))


faptime: $(FAP_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(FAP_OBJS)

all: faptime fcgx

clean:
	rm -rf faptime fcgx obj/ tests/bin


fcgx: $(OBJ_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ_FILES)

test: $(TEST_BINS)
	./$<

obj/%.o: src/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c -o $@ $<

test-bin-folder:
	mkdir -p tests/bin

tests/bin/%.o: tests/%.c test-bin-folder
	$(CC) $(CFLAGS) -c -o $@ $<

%-test: test_%.o $(OBJ_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ obj/$(*F).o $<
