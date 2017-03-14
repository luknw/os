CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

ADDRESSBOOK_PATH = ./libAddressBook

EXECUTABLE_NAME = benchmark

SOURCE_FILES = benchmark.c timer.c
OBJECT_FILES = benchmark.o timer.o

TEST_OUTPUT_FILE = results.txt


all: dynamic shared static


dynamic: $(EXECUTABLE_NAME)_dynamic

$(EXECUTABLE_NAME)_dynamic: $(ADDRESSBOOK_PATH)/libaddressbook.so $(SOURCE_FILES)
# todo


shared: $(EXECUTABLE_NAME)_shared
	
$(EXECUTABLE_NAME)_shared: $(ADDRESSBOOK_PATH)/libaddressbook.so $(SOURCE_FILES)
	$(CC) $(CFLAGS) $(SOURCE_FILES) -L$(ADDRESSBOOK_PATH) -laddressbook -Wl,-rpath=$(ADDRESSBOOK_PATH) -o $@

$(ADDRESSBOOK_PATH)/libaddressbook.so: $(ADDRESSBOOK_PATH)
	$(MAKE) --directory=$^ shared


static: $(EXECUTABLE_NAME)_static

$(EXECUTABLE_NAME)_static: $(ADDRESSBOOK_PATH)/libaddressbook.a $(SOURCE_FILES)
	$(CC) $(CFLAGS) -static $(SOURCE_FILES) -L$(ADDRESSBOOK_PATH) -laddressbook -o $@

$(ADDRESSBOOK_PATH)/libaddressbook.a: $(ADDRESSBOOK_PATH)
	$(MAKE) --directory=$^ static


test: test_dynamic test_shared test_static

test_dynamic: dynamic
	echo "dynamic:" >> $(TEST_OUTPUT_FILE)
	./$(EXECUTABLE_NAME)_dynamic >> $(TEST_OUTPUT_FILE)

test_shared: shared
	echo "shared:" >> $(TEST_OUTPUT_FILE)
	./$(EXECUTABLE_NAME)_shared >> $(TEST_OUTPUT_FILE)

test_static: static
	echo "static:" >> $(TEST_OUTPUT_FILE)
	./$(EXECUTABLE_NAME)_static >> $(TEST_OUTPUT_FILE)


clean:
	$(MAKE) --directory=$(ADDRESSBOOK_PATH) clean
	[ ! -f $(TEST_OUTPUT_FILE) ] || rm $(TEST_OUTPUT_FILE)
	[ ! -f $(EXECUTABLE_NAME)_dynamic ] || rm $(EXECUTABLE_NAME)_dynamic
	[ ! -f $(EXECUTABLE_NAME)_shared ] || rm $(EXECUTABLE_NAME)_shared
	[ ! -f $(EXECUTABLE_NAME)_static ] || rm $(EXECUTABLE_NAME)_static
	find . -maxdepth 1 \( -name "*.o" \) -exec rm {} \;

.PHONY: all dynamic shared static test test_dynamic test_shared test_static clean
