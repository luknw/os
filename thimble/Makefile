CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

SOURCE_FILES = thimble.c liblogger/logger.c libsafe/safeExit.c libsafe/safeAlloc.c libsafe/safeSignal.c

EXECUTABLE_NAME = thimble


all: $(EXECUTABLE_NAME)


$(EXECUTABLE_NAME): $(SOURCE_FILES)
	$(CC) $(CFLAGS) -pthread $^ -o $@


clean:
	rm $(EXECUTABLE_NAME)


.PHONY: all clean
