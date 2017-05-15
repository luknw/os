//
// Created by luknw on 2017-05-10
//

#ifndef NEEDLES_SEWER_H
#define NEEDLES_SEWER_H


#include "libsafe/safe.h"


#define RECORD_SIZE 1024

typedef struct Record Record;

struct Record {
    int id;
    char text[RECORD_SIZE - sizeof(int)];
};


#endif //NEEDLES_SEWER_H
