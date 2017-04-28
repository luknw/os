CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

SERVER_SOURCE_FILES = centralCommittee.c executiveOrders.h libsafe/safeAlloc.c libsafe/safeIO.c liblinkedlist/linkedList.c libhashmap/hashMap.c        
SERVER_EXECUTABLE_NAME = centralCommittee

CLIENT_SOURCE_FILES = regionalCommittee.c executiveOrders.h libsafe/safeAlloc.c libsafe/safeIO.c liblinkedlist/linkedList.c libhashmap/hashMap.c
CLIENT_EXECUTABLE_NAME = regionalCommittee


all: $(SERVER_EXECUTABLE_NAME) $(CLIENT_EXECUTABLE_NAME)

$(SERVER_EXECUTABLE_NAME): $(SERVER_SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT_EXECUTABLE_NAME): $(CLIENT_SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@


clean:
	rm $(SERVER_EXECUTABLE_NAME)
	rm $(CLIENT_EXECUTABLE_NAME)


.PHONY: all clean
