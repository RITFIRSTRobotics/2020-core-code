# core/collections/Makefile
#
# @author Connor Henley, @thatging3rkid
OBJ_DIR=../obj
TEST_OBJ_DIR=../test/obj

CFLAGS=-Wall -Wextra -std=c11 -pthread -lm
TESTFLAGS=-g $(CFLAGS) -D_LOCAL_HEADER
CC=gcc

all: $(OBJ_DIR)/list.o

$(OBJ_DIR)/list.o: list.c list.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

test: list.c ../test/test-list.c list.h
	@mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(TESTFLAGS) -c -o $(TEST_OBJ_DIR)/list.o $<
	@mkdir -p ../tmp
	$(CC) $(TESTFLAGS) $(TEST_OBJ_DIR)/list.o ../test/test-list.c -o ../tmp/test-list
	@echo "!!!!!!!! starting testing"
	@../tmp/test-list
	@valgrind --leak-check=full ../tmp/test-list

clean:
	-/bin/rm -rf $(OBJ_DIR)
