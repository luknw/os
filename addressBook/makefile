CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

ADDRESSBOOK_PATH = ./libaddressbook

EXECUTABLE_NAME = benchmark

SOURCE_FILES = benchmark.c timer.c

TEST_OUTPUT_FILE = results.txt


all: dynamic shared static optimizers


dynamic: $(EXECUTABLE_NAME)_dynamic

$(EXECUTABLE_NAME)_dynamic: $(ADDRESSBOOK_PATH)/libaddressbook.so $(SOURCE_FILES)
	$(CC) $(CFLAGS) $(SOURCE_FILES) -ldl -o $@ -DDYNAMIC_LOADING


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


optimizers: $(EXECUTABLE_NAME)_O1 $(EXECUTABLE_NAME)_O2 $(EXECUTABLE_NAME)_O3 \
			$(EXECUTABLE_NAME)_O0 $(EXECUTABLE_NAME)_Os $(EXECUTABLE_NAME)_Ofast $(EXECUTABLE_NAME)_Og

$(EXECUTABLE_NAME)_O1: $ADDRESSBOOK_PATH/libaddressbook_O1.a $(SOURCE_FILES)
	$(CC) $(CFLAGS) -static $(SOURCE_FILES) -O1 -L$(ADDRESSBOOK_PATH) -laddressbook_O1 -o $@

$ADDRESSBOOK_PATH/libaddressbook_O1.a: $(ADDRESSBOOK_PATH)
	$(MAKE) --directory=$(ADDRESSBOOK_PATH) optimizer_O1

$(EXECUTABLE_NAME)_O2: $ADDRESSBOOK_PATH/libaddressbook_O2.a $(SOURCE_FILES)
	$(CC) $(CFLAGS) -static $(SOURCE_FILES) -O2 -L$(ADDRESSBOOK_PATH) -laddressbook_O2 -o $@

$ADDRESSBOOK_PATH/libaddressbook_O2.a: $(ADDRESSBOOK_PATH)
	$(MAKE) --directory=$(ADDRESSBOOK_PATH) optimizer_O2

$(EXECUTABLE_NAME)_O3: $ADDRESSBOOK_PATH/libaddressbook_O3.a $(SOURCE_FILES)
	$(CC) $(CFLAGS) -static $(SOURCE_FILES) -O3 -L$(ADDRESSBOOK_PATH) -laddressbook_O3 -o $@

$ADDRESSBOOK_PATH/libaddressbook_O3.a: $(ADDRESSBOOK_PATH)
	$(MAKE) --directory=$(ADDRESSBOOK_PATH) optimizer_O3

$(EXECUTABLE_NAME)_O0: $ADDRESSBOOK_PATH/libaddressbook_O0.a $(SOURCE_FILES)
	$(CC) $(CFLAGS) -static $(SOURCE_FILES) -O0 -L$(ADDRESSBOOK_PATH) -laddressbook_O0 -o $@

$ADDRESSBOOK_PATH/libaddressbook_O0.a: $(ADDRESSBOOK_PATH)
	$(MAKE) --directory=$(ADDRESSBOOK_PATH) optimizer_O0

$(EXECUTABLE_NAME)_Os: $ADDRESSBOOK_PATH/libaddressbook_Os.a $(SOURCE_FILES)
	$(CC) $(CFLAGS) -static $(SOURCE_FILES) -Os -L$(ADDRESSBOOK_PATH) -laddressbook_Os -o $@

$ADDRESSBOOK_PATH/libaddressbook_Os.a: $(ADDRESSBOOK_PATH)
	$(MAKE) --directory=$(ADDRESSBOOK_PATH) optimizer_Os

$(EXECUTABLE_NAME)_Ofast: $ADDRESSBOOK_PATH/libaddressbook_Ofast.a $(SOURCE_FILES)
	$(CC) $(CFLAGS) -static $(SOURCE_FILES) -Ofast -L$(ADDRESSBOOK_PATH) -laddressbook_Ofast -o $@

$ADDRESSBOOK_PATH/libaddressbook_Ofast.a: $(ADDRESSBOOK_PATH)
	$(MAKE) --directory=$(ADDRESSBOOK_PATH) optimizer_Ofast

$(EXECUTABLE_NAME)_Og: $ADDRESSBOOK_PATH/libaddressbook_Og.a $(SOURCE_FILES)
	$(CC) $(CFLAGS) -static $(SOURCE_FILES) -Og -L$(ADDRESSBOOK_PATH) -laddressbook_Og -o $@

$ADDRESSBOOK_PATH/libaddressbook_Og.a: $(ADDRESSBOOK_PATH)
	$(MAKE) --directory=$(ADDRESSBOOK_PATH) optimizer_Og


test: test_dynamic test_shared test_static test_optimizers

test_dynamic: dynamic
	echo "dynamic:" >> $(TEST_OUTPUT_FILE)
	./$(EXECUTABLE_NAME)_dynamic >> $(TEST_OUTPUT_FILE)

test_shared: shared
	echo "shared:" >> $(TEST_OUTPUT_FILE)
	./$(EXECUTABLE_NAME)_shared >> $(TEST_OUTPUT_FILE)

test_static: static
	echo "static:" >> $(TEST_OUTPUT_FILE)
	./$(EXECUTABLE_NAME)_static >> $(TEST_OUTPUT_FILE)

test_optimizers: optimizers
	for OPT in 1 2 3 0 s fast g; \
	do \
		echo "optimizer O$$OPT:" >> $(TEST_OUTPUT_FILE); \
		./$(EXECUTABLE_NAME)_O$$OPT >> $(TEST_OUTPUT_FILE); \
	done;



clean:
	$(MAKE) --directory=$(ADDRESSBOOK_PATH) clean
	find . -maxdepth 1 -name "$(EXECUTABLE_NAME)_*" -exec rm {} \;
	find . -maxdepth 1 -name "*.o" -exec rm {} \;
	[ ! -f $(TEST_OUTPUT_FILE) ] || rm $(TEST_OUTPUT_FILE)

.PHONY: all dynamic shared static optimizers test test_dynamic test_shared test_static test_optimizers clean
