CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

MASTER_SOURCE_FILES = oasis.c libsafe/safeAlloc.c libsafe/safeIO.c
MASTER_EXECUTABLE_NAME = oasis

SLAVE_SOURCE_FILES = plan.c libsafe/safeAlloc.c libsafe/safeIO.c
SLAVE_EXECUTABLE_NAME = plan

DATA_FILE_NAME = data


all: $(MASTER_EXECUTABLE_NAME) $(SLAVE_EXECUTABLE_NAME)

$(MASTER_EXECUTABLE_NAME): $(MASTER_SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@

$(SLAVE_EXECUTABLE_NAME): $(SLAVE_SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -lm -o $@


clean:
	rm $(MASTER_EXECUTABLE_NAME)
	rm $(SLAVE_EXECUTABLE_NAME)
	rm data


.PHONY: all clean
