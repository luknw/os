//
// Created by luknw on 3/23/17.
//

#ifndef FLOCKLOCK_MAIN_H
#define FLOCKLOCK_MAIN_H


typedef enum ActionType {
    NON_BLOCKING_READ_LOCK = 1,
    BLOCKING_READ_LOCK,
    NON_BLOCKING_WRITE_LOCK,
    BLOCKING_WRITE_LOCK,
    LIST_LOCKS,
    FREE_LOCK,
    READ_CHAR,
    WRITE_CHAR,
    QUIT
} ActionType;

typedef struct Action Action;

struct Action {
    ActionType type;
    char *description;
};


void nonBlockingReadLock();

void blockingReadLock();

void nonBlockingWriteLock();

void blockingWriteLock();

void freeLock();

void readChar();

void writeChar();

void listLocks(char *filePath);


#endif //FLOCKLOCK_MAIN_H
