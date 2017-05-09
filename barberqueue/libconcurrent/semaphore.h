//
// Created by luknw on 2017-04-30
//

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <assert.h>
#include <stdint.h>



typedef struct Semaphore Semaphore;

struct Semaphore {
    int id;
};


Semaphore *Semaphore_init(Semaphore *s);

void Semaphore_releaseTickets(Semaphore *s, short tickets);

void Semaphore_acquireTickets(Semaphore *s, short tickets);

void Semaphore_setTickets(Semaphore *s, int value);

void Semaphore_await(Semaphore *s);


#endif //SEMAPHORE_H
