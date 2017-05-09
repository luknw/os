//
// Created by luknw on 2017-05-02
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>

#include "utils.h"


static char *const HOME = "HOME";
static char *const DEFAULT_POSIX_IPC_PATH = "/libconcurrent";

static const int PROJ_ID = 'f'; // short for: f**k you ftok

static char *const HOME_DIR_RESOLVE_ERROR = "Error resolving home directory path";
static char *const IPC_KEY_GENERATE_ERROR = "Error generating ipc key";


key_t getDefaultIpcKey(void) {
    char *path = getenv(HOME);
    if (path == NULL) {
        fprintf(stderr, "%s\n", HOME_DIR_RESOLVE_ERROR);
        exit(EXIT_FAILURE);
    }

    key_t key = ftok(path, PROJ_ID);
    if (key == -1) {
        perror(IPC_KEY_GENERATE_ERROR);
        exit(EXIT_FAILURE);
    }

    return key;
}


char *getDefaultIpcPath(void) {
    return DEFAULT_POSIX_IPC_PATH;
}


sembuf sembuf_new(unsigned short semaphoreIndex, short operation, short flags) {
    sembuf a;
    a.sem_num = semaphoreIndex;
    a.sem_op = operation;
    a.sem_flg = flags;

    return a;
}
