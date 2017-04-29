//
// Created by luknw on 4/21/17.
//

#ifndef CENTRALLYPLANNEDECONOMY_CENTRALCOMMITTEE_H
#define CENTRALLYPLANNEDECONOMY_CENTRALCOMMITTEE_H


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>

#include "executiveOrders.h"

#ifndef POSIX_QUEUES

#include <sys/ipc.h>
#include <sys/msg.h>

#else

#include <fcntl.h>
#include <mqueue.h>

#endif

#include "libhashmap/hashMap.h"

#ifndef POSIX_QUEUES
typedef int QueueDescriptor;
#else
typedef mqd_t QueueDescriptor;
#endif

typedef struct Client Client;

struct Client {
    pid_t pid;
    int queueId;
};


#endif //CENTRALLYPLANNEDECONOMY_CENTRALCOMMITTEE_H
