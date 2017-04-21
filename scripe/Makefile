CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

SOURCE_FILES = main.c libsafe/safeIO.c libsafe/safeAlloc.c liblinkedlist/linkedList.c libhashmap/hashMap.c
EXECUTABLE_NAME = scripe

TEST_DATA_FILE = scripet.scp


all: $(EXECUTABLE_NAME)

$(EXECUTABLE_NAME): $(SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@


test: $(EXECUTABLE_NAME) $(TEST_DATA_FILE)
	./$(EXECUTABLE_NAME) $(TEST_DATA_FILE)


clean:
	rm $(EXECUTABLE_NAME)


.PHONY: all test clean
