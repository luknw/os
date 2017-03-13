CC = gcc
CFLAGS = -std=c11 -Wall

LIBRARY_NAME = addressbook
SOURCE_FILES = contact.c listAddressBook.c treeAddressBook.c
OBJECT_FILES = contact.o listAddressBook.o treeAddressBook.o

all: shared static

shared:
	$(CC) $(CFLAGS) -fPIC -shared $(SOURCE_FILES) -o lib$(LIBRARY_NAME).so

static: $(OBJECT_FILES)
	ar rcs lib$(LIBRARY_NAME).a $^

compile: $(SOURCE_FILES)
	$(CC) -c $(CFLAGS) $^

clean:
	find . -maxdepth 1 \( -name "*.[ao]" -o -name "*.so" \) -exec rm {} \;

.PHONY: all shared static compile clean
