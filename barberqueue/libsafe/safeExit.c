//
// Created by luknw on 2017-05-03
//

#include "safeExit.h"

#include <stdlib.h>
#include <stdio.h>


void safe_on_exit(void (*exitHandler)(int exitStatus, void *argument), void *exitHandlerArgument) {
    if (on_exit(exitHandler, exitHandlerArgument) != 0) {
        fprintf(stderr, "%s\n", "Error registering exit handler");
        exit(EXIT_FAILURE);
    }
}
