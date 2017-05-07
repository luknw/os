

#include "condition.h"
#include "../libsafe/safe.h"


static char *const SEMAPHORE_WAIT_ERROR = "Error waiting for semaphore";


#ifndef POSIX_IPC

static sembuf awaitOperations[] = {{0, 0, 0},
                                   {0, 1, 0}};


PoorMansCondition *PoorMansCondition_init(PoorMansCondition *c) {
    Semaphore_init(&c->semaphore);
    Semaphore_setTickets(&c->semaphore, 1);
    return c;
}

void PoorMansCondition_await(PoorMansCondition *c) {
    if (semop(c->semaphore.id, awaitOperations, sizeof(awaitOperations) / sizeof(sembuf)) == -1) {
        if (errno == EINTR) return;

        perror(SEMAPHORE_WAIT_ERROR);
        exit(EXIT_FAILURE);
    }
}

void PoorMansCondition_signal(PoorMansCondition *c) {
    safe_semctl(c->semaphore.id, 0, SETVAL, (semun) {0});
}

#else

#include <semaphore.h>

static char *const SEMAPHORE_INIT_ERROR = "Error initializing semaphore";
static char *const SEMAPHORE_POST_ERROR = "Error posting to semaphore";

static void exit_destroySemaphore(int ignored, void *semaphore) {
    if (sem_destroy(semaphore) == -1) {
        perror("Error destroying POSIX semaphore");
        exit(EXIT_FAILURE);
    }
}


PoorMansCondition *PoorMansCondition_init(PoorMansCondition *c) {
    if (sem_init(&c->condition, 1, 0) == -1) {
        perror(SEMAPHORE_INIT_ERROR);
        exit(EXIT_FAILURE);
    }
    safe_on_exit(exit_destroySemaphore, &c->condition);
    if (sem_init(&c->signalLocker, 1, 1) == -1) {
        perror(SEMAPHORE_INIT_ERROR);
        exit(EXIT_FAILURE);
    }
    safe_on_exit(exit_destroySemaphore, &c->signalLocker);

    return c;
}

void PoorMansCondition_await(PoorMansCondition *c) {
    if (sem_wait(&c->condition) == -1) {
        if(errno == EINTR) return;

        perror(SEMAPHORE_WAIT_ERROR);
        exit(EXIT_FAILURE);
        return;
    }
    if (sem_post(&c->signalLocker) == -1) {
        perror(SEMAPHORE_POST_ERROR);
        exit(EXIT_FAILURE);
    }
}

void PoorMansCondition_signal(PoorMansCondition *c) {
    if (sem_wait(&c->signalLocker) == -1) {
        perror(SEMAPHORE_WAIT_ERROR);
        exit(EXIT_FAILURE);
    }
    if (sem_post(&c->condition) == -1) {
        perror(SEMAPHORE_POST_ERROR);
        exit(EXIT_FAILURE);
    }
}

#endif