//
// Created by luknw on 4/1/17.
//


#ifndef ARRAYQUEUE_H
#define ARRAYQUEUE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../libsafe/safe.h"


typedef struct ArrayQueue ArrayQueue;

struct ArrayQueue {
    void **objects;
    void **front;
    void **back;

    size_t maxObjects;

    size_t len;
};


ArrayQueue *ArrayQueue_new(size_t maxObjects);

void ArrayQueue_delete(ArrayQueue *q);

void ArrayQueue_exit_delete(ArrayQueue *q);

bool ArrayQueue_add(ArrayQueue *q, void *object);

void *ArrayQueue_remove(ArrayQueue *q);

bool ArrayQueue_isEmpty(ArrayQueue *q);


#endif //ARRAYQUEUE_H
