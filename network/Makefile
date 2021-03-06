# core/network/Makefile
#
# @author Connor Henley, @thatging3rkid

# include some default flags
include ../defaults.mk

# setup directories
OBJ_DIR=../obj
TEST_DIR=../test
TEST_OBJ_DIR=$(TEST_DIR)/obj

# Defines the code-files that should force a full re-compile
UTILITY_CODE=constants.h $(OBJ_DIR)/netutils.o

### Build recipes

all: $(OBJ_DIR)/lowlevel.o $(OBJ_DIR)/packethandlers.o

$(OBJ_DIR)/lowlevel.o: lowlevel.c lowlevel.h $(UTILITY_CODE)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/netutils.o: netutils.c netutils.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/packethandlers.o: packethandlers.c packethandlers.h packet.h lowlevel.h netutils.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

### Utility recipes

$(OBJ_DIR)/arraylist.o:
	make -C ../collections/ $@

$(OBJ_DIR)/list.o:
	make -C ../collections/ $@

$(OBJ_DIR)/linkedlist.o:
	make -C ../collections/ $@

### Testing recipes

$(TEST_OBJ_DIR)/test-llnet.o: $(TEST_DIR)/test-llnet.c $(TEST_DIR)/test-utils.h
	@mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TEST_OBJ_DIR)/test-packethandlers.o: $(TEST_DIR)/test-packethandlers.c
	@mkdir -p $(TEST_OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

test-packethandlers: $(OBJ_DIR)/packethandlers.o $(TEST_OBJ_DIR)/test-packethandlers.o $(OBJ_DIR)/arraylist.o $(OBJ_DIR)/lowlevel.o $(OBJ_DIR)/list.o $(OBJ_DIR)/linkedlist.o $(OBJ_DIR)/netutils.o
	$(CC) -o $(TEST_OBJ_DIR)/$@ $^ $(LD_FLAGS)
	@echo "!!!!!!!! starting testing"
	@valgrind --leak-check=full --error-exitcode=1 --suppressions=llnet.valgrind.supp $(TEST_OBJ_DIR)/$@
	@$(TEST_OBJ_DIR)/$@


test-llnet: $(OBJ_DIR)/lowlevel.o $(TEST_OBJ_DIR)/test-llnet.o $(OBJ_DIR)/arraylist.o
	$(CC) -o $(TEST_OBJ_DIR)/test-llnet $^ $(LD_FLAGS)
	@echo "!!!!!!!! starting testing"
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 --suppressions=llnet.valgrind.supp $(TEST_OBJ_DIR)/test-llnet
	@$(TEST_OBJ_DIR)/test-llnet

### CI testing recipes

ci-build: all
ci-test:  test-llnet test-packethandlers
