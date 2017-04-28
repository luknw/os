//
// Created by luknw on 4/21/17.
//

#ifndef CENTRALLYPLANNEDECONOMY_CENTRALCOMMITTEE_H
#define CENTRALLYPLANNEDECONOMY_CENTRALCOMMITTEE_H


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>

#include "libhashmap/hashMap.h"

#include "executiveOrders.h"


typedef struct Client Client;

struct Client {
    pid_t pid;
    int queueId;
};


#endif //CENTRALLYPLANNEDECONOMY_CENTRALCOMMITTEE_H
