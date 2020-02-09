# core/defaults.mk
#
# Makefile defaults shared amongst the project
#
# @author Connor Henley, @thatging3rkid

# Layout the basics
GCC = /usr/bin/gcc
CFLAGS_BASE = -Wall -Wextra -std=c11
CFLAGS_RELEASE = $(CFLAGS_BASE) -O2
CFLAGS_DEBUG = -g $(CFLAGS_BASE)
export LD_FLAGS = -pthread -lm

# Now pick the right set of flags
export CC=$(GCC)
ifdef BUILD_RELEASE
export CFLAGS = $(CFLAGS_RELEASE)
else
export CFLAGS = $(CFLAGS_DEBUG)
endif

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
