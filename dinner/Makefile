CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

SOURCE_FILES = main.c
EXECUTABLE_NAME = dinner


all: $(EXECUTABLE_NAME)

$(EXECUTABLE_NAME): $(SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@

test: $(EXECUTABLE_NAME)
	./$(EXECUTABLE_NAME) 100 0
	./$(EXECUTABLE_NAME) 100 1
	./$(EXECUTABLE_NAME) 100 2

clean:
	rm $(EXECUTABLE_NAME)

.PHONY: all test clean
