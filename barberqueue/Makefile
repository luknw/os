CC = gcc
CFLAGS = -std=gnu11 -Wall -Wpedantic

COMMON_SOURCE_FILES = barbershop.h \
        waitingRoomQueue.c \
        libconcurrent/semaphore.c libconcurrent/semaphore.h \
        libconcurrent/utils.c libconcurrent/utils.h \
        libconcurrent/lock.c libconcurrent/lock.h \
        libconcurrent/condition.c libconcurrent/condition.h \
        libconcurrent/concurrent.h \
        libsafe/safeSignal.c libsafe/safeSignal.h \
        libsafe/safeIpc.c libsafe/safeIpc.h \
        libsafe/safeExit.c libsafe/safeExit.h \
        liblogger/logger.c liblogger/logger.h

BARBER_SOURCE_FILES = $(COMMON_SOURCE_FILES) barber.c waitingRoomQueue.c waitingRoomQueue.h
BARBER_EXECUTABLE_NAME = barber

CLIENT_SOURCE_FILES = $(COMMON_SOURCE_FILES) mothership.c libsafe/safeFork.c libsafe/safeFork.h
CLIENT_EXECUTABLE_NAME = mothership

SYSV = _SYSV
POSIX = _POSIX


all: $(BARBER_EXECUTABLE_NAME)$(SYSV) $(CLIENT_EXECUTABLE_NAME)$(SYSV) $(BARBER_EXECUTABLE_NAME)$(POSIX) $(CLIENT_EXECUTABLE_NAME)$(POSIX)

$(BARBER_EXECUTABLE_NAME)$(SYSV): $(BARBER_SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT_EXECUTABLE_NAME)$(SYSV): $(CLIENT_SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -o $@


$(BARBER_EXECUTABLE_NAME)$(POSIX): $(BARBER_SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -lpthread -DPOSIX_IPC -o $@

$(CLIENT_EXECUTABLE_NAME)$(POSIX): $(CLIENT_SOURCE_FILES)
	$(CC) $(CFLAGS) $^ -lpthread -DPOSIX_IPC -o $@


clean:
	rm $(BARBER_EXECUTABLE_NAME)$(SYSV)
	rm $(CLIENT_EXECUTABLE_NAME)$(SYSV)
	rm $(BARBER_EXECUTABLE_NAME)$(POSIX)
	rm $(CLIENT_EXECUTABLE_NAME)$(POSIX)


.PHONY: all clean
