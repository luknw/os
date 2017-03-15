CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

LIBRARY_NAME = addressbook
SOURCE_FILES = contact.c listAddressBook.c treeAddressBook.c
OBJECT_FILES = $(SOURCE_FILES:.c=.o)


all: shared static 


shared: lib$(LIBRARY_NAME).so
	
lib$(LIBRARY_NAME).so: $(SOURCE_FILES)
	$(CC) $(CFLAGS) -fPIC -shared $^ -o $@


static: lib$(LIBRARY_NAME).a

lib$(LIBRARY_NAME).a: $(OBJECT_FILES)
	ar rcs $@ $^


optimizers: optimizer_O1 optimizer_O2 optimizer_O3 optimizer_O0 optimizer_Os optimizer_Ofast optimizer_Og

optimizer_O1: lib$(LIBRARY_NAME)_O1.a

lib$(LIBRARY_NAME)_O1.a: $(OBJECT_FILES)
	$(CC) -c $(CFLAGS) -O1 $^
	ar rcs $@ $^

optimizer_O2: lib$(LIBRARY_NAME)_O2.a

lib$(LIBRARY_NAME)_O2.a: $(OBJECT_FILES)
	$(CC) -c $(CFLAGS) -O2 $^
	ar rcs $@ $^

optimizer_O3: lib$(LIBRARY_NAME)_O3.a

lib$(LIBRARY_NAME)_O3.a: $(OBJECT_FILES)
	$(CC) -c $(CFLAGS) -O3 $^
	ar rcs $@ $^

optimizer_O0: lib$(LIBRARY_NAME)_O0.a

lib$(LIBRARY_NAME)_O0.a: $(OBJECT_FILES)
	$(CC) -c $(CFLAGS) -O0 $^
	ar rcs $@ $^

optimizer_Os: lib$(LIBRARY_NAME)_Os.a

lib$(LIBRARY_NAME)_Os.a: $(OBJECT_FILES)
	$(CC) -c $(CFLAGS) -Os $^
	ar rcs $@ $^

optimizer_Ofast: lib$(LIBRARY_NAME)_Ofast.a

lib$(LIBRARY_NAME)_Ofast.a: $(OBJECT_FILES)
	$(CC) -c $(CFLAGS) -Ofast $^
	ar rcs $@ $^

optimizer_Og: lib$(LIBRARY_NAME)_Og.a

lib$(LIBRARY_NAME)_Og.a: $(OBJECT_FILES)
	$(CC) -c $(CFLAGS) -Og $^
	ar rcs $@ $^


compile: $(OBJECT_FILES)

$(OBJECT_FILES): $(SOURCE_FILES)
	$(CC) -c $(CFLAGS) $^


clean:
	find . -maxdepth 1 \( -name "*.[ao]" -o -name "*.so" \) -exec rm {} \;


.PHONY: all shared static compile clean \
	optimizers optimizer_O1 optimizer_O2 optimizer_O3 optimizer_O0 optimizer_Os optimizer_Ofast optimizer_Og
