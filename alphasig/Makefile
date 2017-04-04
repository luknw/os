CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

SOURCE_FILES = main.c
EXECUTABLE_NAME = alphasig


all: $(EXECUTABLE_NAME)

$(EXECUTABLE_NAME): $(SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm $(EXECUTABLE_NAME)

.PHONY: all clean
