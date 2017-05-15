CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

SOURCE_FILES = sewer.c sewer.h libsafe/safeExit.c libsafe/safeAlloc.c libsafe/safeIO.c

EXECUTABLE_NAME = sewer


all: $(EXECUTABLE_NAME)


$(EXECUTABLE_NAME): $(SOURCE_FILES)
	$(CC) $(CFLAGS) -pthread $^ -o $@


clean:
	rm $(EXECUTABLE_NAME)


.PHONY: all clean
