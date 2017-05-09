//
// Created by luknw on 2017-05-02
//

#ifndef UTILS_H
#define UTILS_H


#include <sys/ipc.h>
#include <sys/sem.h>


typedef struct sembuf sembuf;


key_t getDefaultIpcKey(void);

char *getDefaultIpcPath(void);

sembuf sembuf_new(unsigned short semaphoreIndex, short operation, short flags);


#endif //UTILS_H
