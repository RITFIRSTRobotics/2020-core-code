# core/collections/Makefile
#
# @author Connor Henley, @thatging3rkid
OBJ_DIR=../obj

TEST_DIR=../test
TEST_OBJ_DIR=$(TEST_DIR)/obj

CFLAGS=-Wall -Wextra -std=c11 -pthread -lm
TESTFLAGS=-g $(CFLAGS) -D_LOCAL_HEADER
CC=gcc

all: $(OBJ_DIR)/arraylist.o $(OBJ_DIR)/linkedlist.o $(OBJ_DIR)/list.o \
	 $(OBJ_DIR)/stack.o $(OBJ_DIR)/queue.o $(OBJ_DIR)/hashtable.o

$(OBJ_DIR)/arraylist.o: arraylist.c arraylist.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/linkedlist.o: linkedlist.c linkedlist.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/list.o: list.c list.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/hashtable.o: hashtable.c hashtable.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/stack.o: stack.c stack.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/queue.o: queue.c queue.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

### Testing recipes

$(TEST_OBJ_DIR)/test-list.o: $(TEST_DIR)/test-list.c
	@mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(TESTFLAGS) -c -o $@ $<

$(TEST_OBJ_DIR)/test-hashtable.o: $(TEST_DIR)/test-hashtable.c
	@mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(TESTFLAGS) -c -o $@ $<

test-list: $(OBJ_DIR)/arraylist.o $(OBJ_DIR)/linkedlist.o $(OBJ_DIR)/list.o \
	  $(OBJ_DIR)/stack.o $(OBJ_DIR)/queue.o $(TEST_OBJ_DIR)/test-list.o
	$(CC) $(TESTFLAGS) $^ -o $(TEST_OBJ_DIR)/test-list
	@echo "!!!!!!!! starting testing"
	@valgrind --leak-check=full --error-exitcode=1 $(TEST_OBJ_DIR)/test-list
	@$(TEST_OBJ_DIR)/test-list

test-hashtable: $(OBJ_DIR)/hashtable.o $(OBJ_DIR)/arraylist.o $(OBJ_DIR)/linkedlist.o $(OBJ_DIR)/list.o $(TEST_OBJ_DIR)/test-hashtable.o
	$(CC) $(TESTFLAGS) $^ -o $(TEST_OBJ_DIR)/test-hashtable
	@echo "!!!!!!!! starting testing"
	@valgrind --leak-check=full --error-exitcode=1 $(TEST_OBJ_DIR)/test-hashtable
	@$(TEST_OBJ_DIR)/test-hashtable

### CI testing stuff

ci-build: all
ci-test: test-list test-hashtable

### Maintenece recipes

clean:
	-/bin/rm -rf $(OBJ_DIR)
	-/bin/rm -rf $(TEST_OBJ_DIR)
