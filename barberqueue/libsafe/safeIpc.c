//
// Created by luknw on 2017-05-03
//

#include "safeIpc.h"

#include <stdlib.h>
#include <stdio.h>


int safe_semget(key_t key, int semaphoreCount, int flags) {
    int semaphoreSetId = semget(key, semaphoreCount, flags);

    if (semaphoreSetId == -1) {
        perror("Error getting sys V semaphore");
        exit(EXIT_FAILURE);
    }

    return semaphoreSetId;
}

void safe_semop(int semaphoreSetId, sembuf *operations, size_t operationCount) {
    if (semop(semaphoreSetId, operations, operationCount) == -1) {
        perror("Error operating on sys V semaphore");
        exit(EXIT_FAILURE);
    }
}

int safe_semctl(int semaphoreSetId, int semaphoreIndex, int command, semun commandDependent) {
    int value = semctl(semaphoreSetId, semaphoreIndex, command, commandDependent);

    if (value == -1) {
        perror("Error performing sys V semaphore control operation");
        exit(EXIT_FAILURE);
    }

    return value;
}


int safe_shmget(key_t key, size_t size, int flags) {
    int sharedMemoryId = shmget(key, size, flags);

    if (sharedMemoryId == -1) {
        perror("Error getting sys V shared memory");
        exit(EXIT_FAILURE);
    }

    return sharedMemoryId;
}

void *safe_shmat(int sharedMemoryId, const void *attachAt, int flags) {
    void *sharedMemory = shmat(sharedMemoryId, attachAt, flags);

    if (sharedMemory == (void *) -1) {
        perror("Error attaching sys V shared memory");
        exit(EXIT_FAILURE);
    }

    return sharedMemory;
}

int safe_shmctl(int sharedMemoryId, int command, shmid_ds *infoBuffer) {
    int value = shmctl(sharedMemoryId, command, infoBuffer);

    if (value == -1) {
        perror("Error performing sys V shared memory control operation");
        exit(EXIT_FAILURE);
    }

    return value;
}
