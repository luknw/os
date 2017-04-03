//
// Created by luknw on 4/1/17.
//

#include "arrayQueue.h"


ArrayQueue *ArrayQueue_new(size_t maxObjects) {
    ArrayQueue *q = safe_malloc(sizeof(ArrayQueue));
    q->objects = safe_calloc(maxObjects, sizeof(void *));

    q->maxObjects = maxObjects;

    q->front = q->objects;
    q->back = q->front;

    q->len = 0;

    return q;
}


void ArrayQueue_delete(ArrayQueue *q) {
    safe_free(q->objects);
    safe_free(q);
}

static void ArrayQueue_advanceObjectPointer(ArrayQueue *q, void ***p) {
    (*p)++;
    if (*p - q->objects == q->maxObjects) {
        *p = q->objects;
    }
}

bool ArrayQueue_add(ArrayQueue *q, void *object) {
    if (q->len + 1 > q->maxObjects) return false;

    (q->len)++;

    *(q->back) = object;
    ArrayQueue_advanceObjectPointer(q, &q->back);

    return true;
}

void *ArrayQueue_remove(ArrayQueue *q) {
    if (q->len - 1 < 0) return NULL;

    (q->len)--;

    void *removed = *(q->front);
    ArrayQueue_advanceObjectPointer(q, &q->front);

    return removed;
}

bool ArrayQueue_isEmpty(ArrayQueue *q) {
    return q->len == 0;
}
