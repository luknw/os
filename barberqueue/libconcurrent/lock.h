//
// Created by luknw on 2017-04-30
//

#ifndef LOCK_H
#define LOCK_H


typedef struct PoorMansLock PoorMansLock;

#ifndef POSIX_IPC

struct PoorMansLock {
    int semaphoreSetId;
};

#else //POSIX_IPC

#include <semaphore.h>

struct PoorMansLock {
    sem_t locker;
};

#endif


PoorMansLock *PoorMansLock_init(PoorMansLock *lock);

void PoorMansLock_lock(PoorMansLock *lock);

void PoorMansLock_unlock(PoorMansLock *lock);


#endif //LOCK_H
