//
// Created by luknw on 4/1/17.
//


#ifndef BARBERQUEUE_WAITING_ROOM_QUEUE_H
#define BARBERQUEUE_WAITING_ROOM_QUEUE_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "libsafe/safe.h"


typedef struct WaitingRoomQueue WaitingRoomQueue;

struct WaitingRoomQueue {
    size_t maxSize;
    size_t size;

    size_t frontIndex;
    size_t backIndex;
    size_t clientsOffset;
};


WaitingRoomQueue *WaitingRoomQueue_init(WaitingRoomQueue *w, size_t maxSize);

bool WaitingRoomQueue_add(WaitingRoomQueue *w, pid_t client);

pid_t WaitingRoomQueue_remove(WaitingRoomQueue *w);

bool WaitingRoomQueue_isEmpty(WaitingRoomQueue *w);


#endif //BARBERQUEUE_WAITING_ROOM_QUEUE_H
