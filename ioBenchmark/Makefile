CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

SOURCE_FILES = ioBenchmark.c libsafealloc/safeAlloc.c libsafefile/safeFile.c
EXECUTABLE_NAME = ioBenchmark

all: $(EXECUTABLE_NAME)

$(EXECUTABLE_NAME): $(SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@

$(EXECUTABLE_NAME)_nftw: $(SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -DUSE_NFTW -o $@

clean:
	rm $(EXECUTABLE_NAME)

.PHONY: all clean
