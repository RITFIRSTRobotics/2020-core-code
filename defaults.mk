# core/defaults.mk
#
# Makefile defaults shared amongst the project
#
# @author Connor Henley, @thatging3rkid
CC=/bin/gcc
CFLAGS=-Wall -Wextra -std=c11 -pthread -lm
TESTFLAGS=-g $(CFLAGS)

all: # empty

# default rule to compile C into an object
# note: experimental and does not account for header files
$(OBJ_DIR)/%.o: %.c 
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean up output directories
.PHONY: clean
clean:
	-/bin/rm -rf $(OBJ_DIR)
	-/bin/rm -rf $(TEST_OBJ_DIR)
