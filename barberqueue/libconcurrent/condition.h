//
// Created by luknw on 2017-04-30
//

#ifndef CONDITION_H
#define CONDITION_H


#include "semaphore.h"

typedef struct PoorMansCondition PoorMansCondition;

#ifndef POSIX_IPC
struct PoorMansCondition {
    Semaphore semaphore;
};
#else

#include <semaphore.h>

struct PoorMansCondition {
    sem_t condition;
    sem_t signalLocker;
};

#endif


PoorMansCondition *PoorMansCondition_init(PoorMansCondition *c);

void PoorMansCondition_await(PoorMansCondition *c);

void PoorMansCondition_signal(PoorMansCondition *c);


#endif //CONDITION_H
