CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

SOURCE_FILES = main.c
EXECUTABLE_NAME = fileInfo

all: $(EXECUTABLE_NAME) $(EXECUTABLE_NAME)_nftw

$(EXECUTABLE_NAME): $(SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@

$(EXECUTABLE_NAME)_nftw: $(SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -DUSE_NFTW -o $@

clean:
	rm $(EXECUTABLE_NAME)
	rm $(EXECUTABLE_NAME)_nftw

.PHONY: all clean
