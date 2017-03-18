//
// Created by luknw on 3/18/17.
//

#include <stdio.h>
#include "safeAlloc.h"


void *safe_calloc(size_t __nmemb, size_t __size) {
    void *allocated = calloc(__nmemb, __size);
    if (allocated == NULL && __nmemb != 0 && __size != 0) {
        perror("Cannot allocate memory: ");
        exit(EXIT_FAILURE);
    }
    return allocated;
}

void safe_free(void *ptr) {
    free(ptr);
}
