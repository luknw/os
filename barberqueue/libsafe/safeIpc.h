//
// Created by luknw on 2017-05-03
//

#ifndef SAFE_IPC_H
#define SAFE_IPC_H


#include <sys/sem.h>
#include <sys/shm.h>


typedef struct sembuf sembuf;
typedef union semun semun;
typedef struct shmid_ds shmid_ds;

///copied from manual
union semun {
    int val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array;  /* Array for GETALL, SETALL */
    struct seminfo *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};


int safe_semget(key_t key, int semaphoreCount, int flags);

void safe_semop(int semaphoreSetId, sembuf *operations, size_t operationCount);

int safe_semctl(int semaphoreSetId, int semaphoreIndex, int command, semun commandDependent);


int safe_shmget(key_t key, size_t size, int flags);

void *safe_shmat(int sharedMemoryId, const void *attachAt, int flags);

int safe_shmctl(int sharedMemoryId, int command, shmid_ds *infoBuffer);


#endif //SAFE_IPC_H
