CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

SOURCE_FILES = main.c libsafe/safeIO.c libsafe/safeAlloc.c libarrayqueue/arrayQueue.c
EXECUTABLE_NAME = gandalf


all: $(EXECUTABLE_NAME)

$(EXECUTABLE_NAME): $(SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@

test: $(EXECUTABLE_NAME)
	./$(EXECUTABLE_NAME) 9 8

clean:
	rm $(EXECUTABLE_NAME)

.PHONY: all test clean
