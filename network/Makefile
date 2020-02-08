# core/network/Makefile
#
# @author Connor Henley, @thatging3rkid

# include some default flags
include ../defaults.mk

OBJ_DIR=../obj
TEST_DIR=../test
TEST_OBJ_DIR=$(TEST_DIR)/obj

CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -pthread -lm
TESTFLAGS=-g $(CFLAGS)

# Defines the code-files that should force a full re-compile
UTILITY_CODE=constants.h $(OBJ_DIR)/netutils.o

### Build recipes

all: $(OBJ_DIR)/lowlevel.o

$(OBJ_DIR)/lowlevel.o: lowlevel.c lowlevel.h $(UTILITY_CODE)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/netutils.o: netutils.c netutils.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

### Testing recipes

$(TEST_OBJ_DIR)/test-llnet.o: $(TEST_DIR)/test-llnet.c $(TEST_DIR)/test-utils.h
	@mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(TESTFLAGS) -c -o $@ $<

test-llnet: $(OBJ_DIR)/lowlevel.o $(TEST_OBJ_DIR)/test-llnet.o $(OBJ_DIR)/arraylist.o
	$(CC) $(TESTFLAGS) -o $(TEST_OBJ_DIR)/test-llnet $^ -lm # gcc wants -lm at the end
	@echo "!!!!!!!! starting testing"
	@valgrind --leak-check=full --error-exitcode=1 $(TEST_OBJ_DIR)/test-llnet
	@$(TEST_OBJ_DIR)/test-llnet

### CI testing recipes

ci-build: all
ci-test:  test-llnet
