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

all: ; # empty

### Testing recipes

### CI testing recipes

ci-build: ; # empty
ci-test:  ; # empty

### Maintnence recipes

clean:
	-/bin/rm -rf $(OBJ_DIR)
	-/bin/rm -rf $(TEST_OBJ_DIR)
