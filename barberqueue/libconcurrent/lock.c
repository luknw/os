//
// Created by luknw on 2017-04-30
//


#include "lock.h"

#ifndef POSIX_IPC

#include <sys/stat.h>
#include <stdint.h>

#include "../libsafe/safe.h"


static char *const SEMAPHORE_REMOVE_ERROR = "Error removing sys V unlockSignaler";

enum SemaphoreIndices {
    LOCKER,
    UNLOCK_SIGNALER
};

static sembuf lockOperations[] = {{LOCKER,          -1, 0},
                                  {UNLOCK_SIGNALER, 1,  0}};
static sembuf unlockOperations[] = {{LOCKER,          1,  0},
                                    {UNLOCK_SIGNALER, -1, 0}};


static void exit_removeSysVSemaphore(int ignored, void *sysVSemaphoreId) {
    if (semctl((int) (uintptr_t) sysVSemaphoreId, 0, IPC_RMID) == -1) {
        perror(SEMAPHORE_REMOVE_ERROR);
    }
}

PoorMansLock *PoorMansLock_init(PoorMansLock *lock) {
    lock->semaphoreSetId = safe_semget(IPC_PRIVATE, 2, S_IRUSR | S_IWUSR);
    safe_on_exit(exit_removeSysVSemaphore, (void *) (uintptr_t) lock->semaphoreSetId);

    safe_semctl(lock->semaphoreSetId, 0, SETVAL, (semun) {1});
    return lock;
}

void PoorMansLock_lock(PoorMansLock *lock) {
    safe_semop(lock->semaphoreSetId, lockOperations, sizeof(lockOperations) / sizeof(sembuf));
}

void PoorMansLock_unlock(PoorMansLock *lock) {
    safe_semop(lock->semaphoreSetId, unlockOperations, sizeof(unlockOperations) / sizeof(sembuf));
}

#else //POSIX_IPC

#include <stdio.h>
#include <stdlib.h>

#include "../libsafe/safe.h"


static char *const SEMAPHORE_INIT_ERROR = "Error initializing semaphore";


static void exit_destroySemaphore(int ignored, void *semaphore) {
    if (sem_destroy(semaphore) == -1) {
        perror("Error destroying POSIX semaphore");
        exit(EXIT_FAILURE);
    }
}

PoorMansLock *PoorMansLock_init(PoorMansLock *lock) {
    if (sem_init(&lock->locker, 1, 1) == -1) {
        perror(SEMAPHORE_INIT_ERROR);
        exit(EXIT_FAILURE);
    }
    safe_on_exit(exit_destroySemaphore, &lock->locker);
    return lock;
}

void PoorMansLock_lock(PoorMansLock *lock) {
    if (sem_wait(&lock->locker) == -1) {
        perror("Error acquiring POSIX lock");
        exit(EXIT_FAILURE);
    }
}

void PoorMansLock_unlock(PoorMansLock *lock) {
    if (sem_post(&lock->locker)) {
        perror("Error releasing POSIX lock");
        exit(EXIT_FAILURE);
    }
}

#endif //POSIX_IPC