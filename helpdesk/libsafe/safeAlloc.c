//
// Created by luknw on 3/18/17.
//

#include "safeAlloc.h"


void *safe_malloc(size_t size) {
    void *allocated = malloc(size);
    if (allocated == NULL && size != 0) {
        perror("Cannot allocate memory");
        exit(EXIT_FAILURE);
    }
    return allocated;
}

void *safe_calloc(size_t count, size_t size) {
    void *allocated = calloc(count, size);
    if (allocated == NULL && count != 0 && size != 0) {
        perror("Cannot allocate memory");
        exit(EXIT_FAILURE);
    }
    return allocated;
}

void safe_free(void *ptr) {
    free(ptr);
}

void *safe_realloc(void *ptr, size_t size) {
    void *allocated = realloc(ptr, size);
    if (allocated == NULL && size != 0) {
        perror("Cannot allocate memory");
        exit(EXIT_FAILURE);
    }
    return allocated;
}
