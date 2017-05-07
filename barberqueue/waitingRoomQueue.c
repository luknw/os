//
// Created by luknw on 4/1/17.
//

#include "waitingRoomQueue.h"


static pid_t *WaitingRoomQueue_resolveAddress(WaitingRoomQueue *w, size_t index) {
    return (pid_t *) (((char *) w) + w->clientsOffset + sizeof(pid_t) * index);
}

WaitingRoomQueue *WaitingRoomQueue_init(WaitingRoomQueue *w, size_t maxSize) {
    w->maxSize = maxSize;
    w->size = 0;

    w->clientsOffset = sizeof(*w);

    w->backIndex = w->frontIndex = 0;

    return w;
}

static void WaitingRoomQueue_advanceClientIndex(WaitingRoomQueue *w, size_t *index) {
    (*index)++;
    *index %= w->maxSize;
}

bool WaitingRoomQueue_add(WaitingRoomQueue *w, pid_t client) {
    if (w->size >= w->maxSize) return false;

    (w->size)++;

    pid_t *back = WaitingRoomQueue_resolveAddress(w, w->backIndex);
    *back = client;
    WaitingRoomQueue_advanceClientIndex(w, &w->backIndex);

    return true;
}

pid_t WaitingRoomQueue_remove(WaitingRoomQueue *w) {
    if (w->size == 0) return 0;

    (w->size)--;

    pid_t removed = *WaitingRoomQueue_resolveAddress(w, w->frontIndex);
    WaitingRoomQueue_advanceClientIndex(w, &w->frontIndex);

    return removed;
}

bool WaitingRoomQueue_isEmpty(WaitingRoomQueue *w) {
    return w->size == 0;
}
