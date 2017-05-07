//
// Created by luknw on 2017-05-04
//

#ifndef LOGGER_H
#define LOGGER_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>


#define MICROSECONDS_PER_SECOND 1000000


#define MLOG(formattedMessage, ...) { \
            struct timespec timestamp;\
            if (clock_gettime(CLOCK_MONOTONIC_RAW, &timestamp) == -1) {\
                perror("Error getting time");\
                exit(EXIT_FAILURE);\
            }\
            printf("[%ld.%06ld] %d: " formattedMessage "\n", \
                timestamp.tv_sec, timestamp.tv_nsec % MICROSECONDS_PER_SECOND, getpid(), __VA_ARGS__);\
        }

void mlog(char *message);


#endif //LOGGER_H
