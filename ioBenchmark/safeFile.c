//
// Created by luknw on 3/18/17.
//

#include "safeFile.h"


FILE *safe_fopen(const char *__restrict __filename, const char *__restrict __modes) {
    FILE *f = fopen(__filename, __modes);
    if (f == NULL) {
        perror("Error opening file: ");
        exit(EXIT_FAILURE);
    }
    return f;
}