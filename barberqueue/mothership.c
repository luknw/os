#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>


#ifdef POSIX_IPC

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#endif

#include "libsafe/safe.h"
#include "liblogger/logger.h"
#include "barbershop.h"


static char *const USAGE = "Gimme 2 of'em numbers. Bastard count and barbings for each one of'em";
static const int NANOSECONDS_PER_SECOND = 1000000000;

static char *const BARBER_WAKE_UP_MSG = "Waking barber up";
static char *const CLIENT_WAIT_MSG = "Waiting for the cut";
static char *const WAITING_ROOM_FULL_MSG = "No place in the waiting room";
static char *const CLIENT_LEAVES_MSG = "Had a nice hair cut";


void spawnLongHairedBastard(Barbershop *barbershop, unsigned int barbings, sigset_t clientCutSignals) {
    pid_t maybeChildPid = safe_fork();
    if (maybeChildPid > 0) return;

    time_t randSeed = time(NULL);
    if (randSeed == -1) {
        perror("Error getting time");
        exit(EXIT_FAILURE);
    }
    srand((unsigned int) (randSeed ^ getpid()));

    while (barbings--) {
        PoorMansLock_lock(&barbershop->waitingRoomLock);

        if (!WaitingRoomQueue_add(&barbershop->waitingRoomQueue, getpid())) {
            mlog(WAITING_ROOM_FULL_MSG);
            barbings++;
            PoorMansLock_unlock(&barbershop->waitingRoomLock);

            struct timespec sleepTime = {0, rand() % NANOSECONDS_PER_SECOND};
            if (nanosleep(&sleepTime, NULL) == -1) {
                perror("Error sleeping");
                exit(EXIT_FAILURE);
            }

            continue;
        }

        sigset_t oldSignalSet;
        safe_sigprocmask(SIG_BLOCK, &clientCutSignals, &oldSignalSet);


        mlog(CLIENT_WAIT_MSG);
        PoorMansLock_unlock(&barbershop->waitingRoomLock);

        mlog(BARBER_WAKE_UP_MSG);
        PoorMansCondition_signal(&barbershop->clientReady);

        int deliveredSignal;
        while (safe_sigwait(&clientCutSignals, &deliveredSignal), deliveredSignal != CLIENT_CUT_SIGNAL);

        mlog(CLIENT_LEAVES_MSG);
        safe_sigprocmask(SIG_SETMASK, &oldSignalSet, NULL);
    }
    exit(EXIT_SUCCESS);
}


void reheatDinnerUntilBastardsComeBack(void) {
    int oldErrno = errno;
    errno = 0;

    int status;
    while (wait(&status) > 0 && WEXITSTATUS(status) == 0);
    if (errno != 0 && errno != ECHILD) {
        perror("Error waiting for children");
        exit(EXIT_FAILURE);
    }

    errno = oldErrno;
}


static sigset_t getClientCutSignals(void) {
    sigset_t clientCut;
    safe_sigemptyset(&clientCut);
    safe_sigaddset(&clientCut, CLIENT_CUT_SIGNAL);
    return clientCut;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "%s\n", USAGE);
        exit(EXIT_FAILURE);
    }

    unsigned int bastards = (unsigned int) atol(argv[1]);
    unsigned int barbings = (unsigned int) atol(argv[2]);

    sigset_t clientCutSignals = getClientCutSignals();

#ifndef POSIX_IPC
    Barbershop *barbershop = safe_shmat(safe_shmget(getDefaultIpcKey(), 0, 0), NULL, 0);
#else
    int shmId = safe_shm_open(getDefaultIpcPath(), O_RDWR, S_IRUSR | S_IWUSR);
    Barbershop *barbershop = safe_mmap(NULL, sizeof(Barbershop), PROT_READ | PROT_WRITE,
                                       MAP_SHARED, shmId, 0);
    size_t fullBarbershopSize = sizeof(Barbershop) + sizeof(pid_t) * barbershop->waitingRoomQueue.maxSize;
    safe_close(shmId);
    shmId = safe_shm_open(getDefaultIpcPath(), O_RDWR, S_IRUSR | S_IWUSR);
    barbershop = safe_mmap(NULL, fullBarbershopSize, PROT_READ | PROT_WRITE,
                           MAP_SHARED, shmId, 0);
#endif

    while (bastards--) {
        spawnLongHairedBastard(barbershop, barbings, clientCutSignals);
    }
    reheatDinnerUntilBastardsComeBack();

    return 0;
}
