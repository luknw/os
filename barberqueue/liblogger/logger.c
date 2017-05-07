//
// Created by luknw on 2017-05-04
//

#include "logger.h"


typedef struct timespec timespec;


void mlog(char *message) {
    timespec timestamp;
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &timestamp) == -1) {
        perror("Error getting time");
        exit(EXIT_FAILURE);
    }

    printf("[%ld.%06ld] %d: %s\n", timestamp.tv_sec, timestamp.tv_nsec % MICROSECONDS_PER_SECOND, getpid(), message);
}
