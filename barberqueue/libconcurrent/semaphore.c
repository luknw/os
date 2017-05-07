//
// Created by luknw on 2017-04-30
//

#include "semaphore.h"


static char *const SEMAPHORE_CREATE_ERROR = "Error creating sys V unlockSignaler";
static char *const SEMAPHORE_REMOVE_ERROR = "Error removing sys V unlockSignaler";
static char *const SEMAPHORE_TICKETS_ACQUIRE_ERROR = "Error acquiring unlockSignaler tickets";
static char *const SEMAPHORE_TICKETS_RELEASE_ERROR = "Error releasing unlockSignaler tickets";
static char *const SEMAPHORE_TICKETS_AWAIT_NONE_ERROR = "Error awaiting no unlockSignaler tickets";
static char *const SEMAPHORE_TICKETS_SET_ERROR = "Error setting unlockSignaler tickets";


static void exit_removeSysVSemaphore(int ignored, void *sysVSemaphoreId) {
    if (semctl((int) sysVSemaphoreId, 0, IPC_RMID) == -1) {
        perror(SEMAPHORE_REMOVE_ERROR);
    }
}

Semaphore *Semaphore_init(Semaphore *s) {
    s->id = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
    if (s->id == -1) {
        perror(SEMAPHORE_CREATE_ERROR);
        exit(EXIT_FAILURE);
    }
    on_exit(exit_removeSysVSemaphore, (void *) s->id);

    return s;
}

void Semaphore_releaseTickets(Semaphore *s, short tickets) {
    assert(tickets > 0);

    struct sembuf increment;
    increment.sem_num = 0;
    increment.sem_op = tickets;
    increment.sem_flg = 0;

    if (semop(s->id, &increment, 1) == -1) {
        perror(SEMAPHORE_TICKETS_RELEASE_ERROR);
        exit(EXIT_FAILURE);
    }
}

void Semaphore_acquireTickets(Semaphore *s, short tickets) {
    assert(tickets > 0);

    struct sembuf decrement;
    decrement.sem_num = 0;
    decrement.sem_op = -tickets;
    decrement.sem_flg = 0;

    if (semop(s->id, &decrement, 1) == -1) {
        perror(SEMAPHORE_TICKETS_ACQUIRE_ERROR);
        exit(EXIT_FAILURE);
    }
}

void Semaphore_setTickets(Semaphore *s, int value) {
    assert(value >= 0);

    ///copied from manual
    union semun {
        int val;    /* Value for SETVAL */
        struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
        unsigned short *array;  /* Array for GETALL, SETALL */
        struct seminfo *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
    } setVal;
    setVal.val = value;

    if (semctl(s->id, 0, SETVAL, setVal) == -1) {
        perror(SEMAPHORE_TICKETS_SET_ERROR);
        exit(EXIT_FAILURE);
    }
}

void Semaphore_await(Semaphore *s) {
    struct sembuf awaitNoTickets;
    awaitNoTickets.sem_num = 0;
    awaitNoTickets.sem_op = 0;
    awaitNoTickets.sem_flg = 0;

    if (semop(s->id, &awaitNoTickets, 1) == -1) {
        perror(SEMAPHORE_TICKETS_AWAIT_NONE_ERROR);
        exit(EXIT_FAILURE);
    }
}

