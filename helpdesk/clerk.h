//
// Created by luknw on 2017-05-27
//

#ifndef HELPDESK_CLERK_H
#define HELPDESK_CLERK_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include "libsafe/safeIO.h"
#include "libsafe/safeSignal.h"
#include "liblogger/logger.h"
#include "protocol.h"


typedef struct Worker Worker;

struct Worker {
    int fd;
    char name[MAX_CLIENT_NAME_LEN];
    bool responsive;
};


#endif //HELPDESK_CLERK_H
