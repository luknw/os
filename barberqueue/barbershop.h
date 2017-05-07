//
// Created by luknw on 2017-04-30
//

#ifndef BARBERQUEUE_BARBERSHOP_H
#define BARBERQUEUE_BARBERSHOP_H


#include <stdlib.h>
#include <signal.h>

#include "waitingRoomQueue.h"
#include "libconcurrent/concurrent.h"


#define CLIENT_CUT_SIGNAL SIGUSR1

typedef struct Barbershop Barbershop;

struct Barbershop {
    PoorMansCondition clientReady;
    PoorMansLock waitingRoomLock;
    WaitingRoomQueue waitingRoomQueue;
};


#endif //BARBERQUEUE_BARBERSHOP_H
