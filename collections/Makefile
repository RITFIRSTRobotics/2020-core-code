# core/collections/Makefile
#
# @author Connor Henley, @thatging3rkid
OBJ_DIR=../obj

TEST_DIR=../test
TEST_OBJ_DIR=$(TEST_DIR)/obj

CFLAGS=-Wall -Wextra -std=c11 -pthread -lm
TESTFLAGS=-g $(CFLAGS) -D_LOCAL_HEADER
CC=gcc

all: $(OBJ_DIR)/arraylist.o $(OBJ_DIR)/linkedlist.o $(OBJ_DIR)/list.o

$(OBJ_DIR)/arraylist.o: arraylist.c arraylist.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/linkedlist.o: linkedlist.c linkedlist.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/list.o: list.c list.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

### Testing recipes

$(TEST_OBJ_DIR)/arraylist.o: arraylist.c arraylist.h
	@mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(TESTFLAGS) -c -o $@ $<

$(TEST_OBJ_DIR)/linkedlist.o: linkedlist.c linkedlist.h
	@mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(TESTFLAGS) -c -o $@ $<

$(TEST_OBJ_DIR)/list.o: list.c list.h
	@mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(TESTFLAGS) -c -o $@ $<

$(TEST_OBJ_DIR)/stack.o: stack.c stack.h
	@mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(TESTFLAGS) -c -o $@ $<

$(TEST_OBJ_DIR)/queue.o: queue.c queue.h
	@mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(TESTFLAGS) -c -o $@ $<

$(TEST_OBJ_DIR)/test-list.o: $(TEST_DIR)/test-list.c
	@mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(TESTFLAGS) -c -o $@ $<

test: $(TEST_OBJ_DIR)/arraylist.o $(TEST_OBJ_DIR)/linkedlist.o $(TEST_OBJ_DIR)/list.o \
	  $(TEST_OBJ_DIR)/stack.o $(TEST_OBJ_DIR)/queue.o $(TEST_OBJ_DIR)/test-list.o
	$(CC) $(TESTFLAGS) $^ -o $(TEST_OBJ_DIR)/test-list
	@echo "!!!!!!!! starting testing"
	@$(TEST_OBJ_DIR)/test-list
	@valgrind --leak-check=full --error-exitcode=1 $(TEST_OBJ_DIR)/test-list

### CI testing stuff

ci-all: $(TEST_OBJ_DIR)/arraylist.o $(TEST_OBJ_DIR)/linkedlist.o $(TEST_OBJ_DIR)/list.o \
	    $(TEST_OBJ_DIR)/stack.o $(TEST_OBJ_DIR)/queue.o

ci-test: test

### Maintenece recipes

clean:
	-/bin/rm -rf $(OBJ_DIR)
	-/bin/rm -rf $(TEST_OBJ_DIR)
