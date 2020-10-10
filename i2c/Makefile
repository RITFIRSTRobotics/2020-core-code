# core/i2c/Makefile
#
# @author Connor Henley, @thatging3rkid

# include some default flags
include ../defaults.mk

# setup directories
OBJ_DIR=../obj
TEST_DIR=../test
TEST_OBJ_DIR=$(TEST_DIR)/obj

### Build recipes

all: $(OBJ_DIR)/i2cUtils.o

$(OBJ_DIR)/i2cUtils.o: i2cUtils.c i2cUtils.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

### CI testing recipes

ci-build: all
ci-test:  ; @# do nothing
