# core/network/Makefile
#
# @author Connor Henley, @thatging3rkid
OBJ_DIR=../obj
TEST_DIR=../test
TEST_OBJ_DIR=$(TEST_DIR)/obj

CFLAGS=-Wall -Wextra -std=c11 -pthread -lm
TESTFLAGS=-g $(CFLAGS) -D_LOCAL_HEADER
CC=gcc

### Build recipes

all: $(OBJ_DIR)/net_lowlevel.o

$(OBJ_DIR)/net_lowlevel.o: lowlevel.c constants.h lowlevel.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

### Testing recipes

### CI testing recipes

ci-build: all
ci-test:  ; # empty

### Maintnence recipes

clean:
	-/bin/rm -rf $(OBJ_DIR)
	-/bin/rm -rf $(TEST_OBJ_DIR)
