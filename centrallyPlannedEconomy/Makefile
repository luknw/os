CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

SERVER_SOURCE_FILES = centralCommittee.c executiveOrders.h libsafe/safeAlloc.c libsafe/safeIO.c liblinkedlist/linkedList.c libhashmap/hashMap.c        
SERVER_EXECUTABLE_NAME = centralCommittee

CLIENT_SOURCE_FILES = regionalCommittee.c executiveOrders.h libsafe/safeAlloc.c libsafe/safeIO.c liblinkedlist/linkedList.c libhashmap/hashMap.c
CLIENT_EXECUTABLE_NAME = regionalCommittee

IPC = _IPC
POSIX = _POSIX


all: $(SERVER_EXECUTABLE_NAME)$(IPC) $(CLIENT_EXECUTABLE_NAME)$(IPC) $(SERVER_EXECUTABLE_NAME)$(POSIX) $(CLIENT_EXECUTABLE_NAME)$(POSIX)

$(SERVER_EXECUTABLE_NAME)$(IPC): $(SERVER_SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT_EXECUTABLE_NAME)$(IPC): $(CLIENT_SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@


$(SERVER_EXECUTABLE_NAME)$(POSIX): $(SERVER_SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -lrt -DPOSIX_QUEUES -o $@

$(CLIENT_EXECUTABLE_NAME)$(POSIX): $(CLIENT_SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -lrt -DPOSIX_QUEUES -o $@


clean:
	rm $(SERVER_EXECUTABLE_NAME)$(IPC)
	rm $(CLIENT_EXECUTABLE_NAME)$(IPC)
	rm $(SERVER_EXECUTABLE_NAME)$(POSIX)
	rm $(CLIENT_EXECUTABLE_NAME)$(POSIX)


.PHONY: all clean
