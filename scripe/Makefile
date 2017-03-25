CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

SOURCE_FILES = main.c libsafe/safeIO.c libsafe/safeAlloc.c
EXECUTABLE_NAME = scripe

TEST_SOURCE_FILES = environTester.c
TEST_EXECUTABLE_NAME = environTester

TEST_DATA_FILE = scripet


all: $(EXECUTABLE_NAME)

$(EXECUTABLE_NAME): $(SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@


test: $(EXECUTABLE_NAME) $(TEST_EXECUTABLE_NAME) $(TEST_DATA_FILE)
	./$(EXECUTABLE_NAME) $(TEST_DATA_FILE)

$(TEST_EXECUTABLE_NAME): $(TEST_SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@


clean:
	rm $(EXECUTABLE_NAME)
	rm $(TEST_EXECUTABLE_NAME)

.PHONY: all test clean
