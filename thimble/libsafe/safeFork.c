//
// Created by luknw on 2017-05-03
//

#include "safeFork.h"

#include <stdio.h>
#include <unistd.h>


pid_t safe_fork(void) {
    pid_t maybeChildPid = fork();

    if (maybeChildPid == -1) {
        perror("Error while forking");
        exit(EXIT_FAILURE);
    }

    return maybeChildPid;
}
