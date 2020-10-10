# core/<FIX_ME>/Makefile
#
# @author Connor Henley, @thatging3rkid
# @author <FIX_ME>

# include some default flags
include ../defaults.mk

# setup directories
OBJ_DIR=../obj
TEST_DIR=../test
TEST_OBJ_DIR=$(TEST_DIR)/obj

### Build recipes

all: $(OBJ_DIR)/somefile.o

$(OBJ_DIR)/somefile.o: somefile.c somefile.h someutil.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

### Utility recipes

$(OBJ_DIR)/external.o:
	make -C ../location/ $@

### Testing recipes

$(TEST_OBJ_DIR)/test-somefile.o: $(TEST_DIR)/test-somefile.c $(TEST_DIR)/test-someutil.h
	@mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

test-somefile: $(TEST_OBJ_DIR)/test-packethandlers.o $(OBJ_DIR)/somefile.o
	$(CC) -o $(TEST_OBJ_DIR)/$@ $^ $(LD_FLAGS)
	@echo "!!!!!!!! starting testing"
	@valgrind --leak-check=full --error-exitcode=1 $(TEST_OBJ_DIR)/$@
	@$(TEST_OBJ_DIR)/$@

### CI testing recipes

ci-build: all
ci-test:  test-somefile
