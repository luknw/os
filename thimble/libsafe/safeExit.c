//
// Created by luknw on 2017-05-03
//

#include "safeExit.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


void exit_freeWrapper(int status, void *ptr) {
    free(ptr);
}

void exit_closeWrapper(int status, void *fd) {
    if (close((int) (uintptr_t) fd) == -1) {
        perror("Error closing file descriptor");
    }
}

void safe_on_exit(void (*exitHandler)(int exitStatus, void *argument), void *exitHandlerArgument) {
    if (on_exit(exitHandler, (void *) exitHandlerArgument) != 0) {
        fprintf(stderr, "%s\n", "Error registering exit handler");
        exit(EXIT_FAILURE);
    }
}
