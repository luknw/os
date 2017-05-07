#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <time.h>

#include "libsafe/safe.h"
#include "liblogger/logger.h"

#include "barbershop.h"


static char *const ABUSAGE = "Oy man, need to know me seat number";


static char const SIGNAL_HANDLER_MSG[] = "signal handler\n";
#define BARBER_SLEEP_MSG "Barber enters sleep"
#define START_CLIENT_CUT_MSG "Barber starts cutting Mr. %d"
#define END_CLIENT_CUT_MSG "Barber ends cutting Mr. %d"

static bool kitkat;

static void signalHandler_dispenseKitkat(int ignored) {
//    if (write(STDOUT_FILENO, SIGNAL_HANDLER_MSG, sizeof(SIGNAL_HANDLER_MSG)) == -1) {
//        perror("Error handling interrupt signal");
//    }
    kitkat = true;
}


static void exit_removeSharedMemory(int ignored, void *sharedMemoryId) {
    safe_shmctl((int) sharedMemoryId, IPC_RMID, NULL);
}

Barbershop *openBarbershop(unsigned int seatCount) {
    kitkat = false;

    sigset_t empty;
    safe_sigemptyset(&empty);

    struct sigaction handlerInfo;
    handlerInfo.sa_handler = signalHandler_dispenseKitkat;
    handlerInfo.sa_mask = empty;
    handlerInfo.sa_flags = 0;

    safe_sigaction(SIGINT, &handlerInfo, NULL);

    size_t barbershopSize = sizeof(Barbershop) + (seatCount + 1) * sizeof(pid_t);

    int sharedMemoryId = safe_shmget(getDefaultIpcKey(), barbershopSize, IPC_CREAT | S_IRUSR | S_IWUSR);
    safe_on_exit(exit_removeSharedMemory, (void *) sharedMemoryId);

    Barbershop *b = safe_shmat(sharedMemoryId, NULL, 0);
    PoorMansCondition_init(&b->clientReady);
    PoorMansLock_init(&b->waitingRoomLock);
    WaitingRoomQueue_init(&b->waitingRoomQueue, seatCount + 1);

    return b;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "%s\n", ABUSAGE);
        exit(EXIT_FAILURE);
    }
    unsigned int seatCount = (unsigned int) atol(argv[1]);

    Barbershop *b = openBarbershop(seatCount);

    mlog(BARBER_SLEEP_MSG);
    PoorMansCondition_await(&b->clientReady);

    while (!kitkat) {
        PoorMansLock_lock(&b->waitingRoomLock);

        while (!WaitingRoomQueue_isEmpty(&b->waitingRoomQueue)) {
            pid_t client = WaitingRoomQueue_remove(&b->waitingRoomQueue);
            PoorMansLock_unlock(&b->waitingRoomLock);

            MLOG(START_CLIENT_CUT_MSG, client);
            safe_kill(client, CLIENT_CUT_SIGNAL);
            MLOG(END_CLIENT_CUT_MSG, client);

            PoorMansLock_lock(&b->waitingRoomLock);
        }

        PoorMansLock_unlock(&b->waitingRoomLock);

        mlog(BARBER_SLEEP_MSG);
        PoorMansCondition_await(&b->clientReady);
    }

    return 0;
}
