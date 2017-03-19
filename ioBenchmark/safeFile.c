//
// Created by luknw on 3/18/17.
//

#include <string.h>
#include <errno.h>

#include "safeFile.h"


FILE *safe_fopen(const char *__restrict filename, const char *__restrict modes) {
    FILE *f = fopen(filename, modes);
    if (f == NULL) {
        char *errorString = strerror(errno);
        fprintf(stderr, "Error opening file: %s\n%s", filename, errorString);
        exit(EXIT_FAILURE);
    }
    return f;
}

int safe_fclose(FILE *stream) {
    int status = fclose(stream);
    if (status == EOF) {
        perror("Error closing file: ");
        exit(EXIT_FAILURE);
    }
    return status;
}

int safe_fseek(FILE *stream, long int offset, int whence) {
    int status = fseek(stream, offset, whence);
    if (status == -1) {
        perror("Error operating on file: ");
        exit(EXIT_FAILURE);
    }
    return status;
}

void safe_rewind(FILE *stream) {
    rewind(stream);
}

int safe_fgetpos(FILE *__restrict stream, fpos_t *__restrict position) {
    int status = fgetpos(stream, position);
    if (status == -1) {
        perror("Error operating on file: ");
        exit(EXIT_FAILURE);
    }
    return status;
}

int safe_fsetpos(FILE *stream, const fpos_t *position) {
    int status = fsetpos(stream, position);
    if (status == -1) {
        perror("Error operating on file: ");
        exit(EXIT_FAILURE);
    }
    return status;
}

size_t safe_fread(void *__restrict target, size_t size, size_t count, FILE *__restrict file) {
    size_t itemsRead = fread(target, size, count, file);
    if (itemsRead < count) {
        fprintf(stderr, "Error reading file\n");
        exit(EXIT_FAILURE);
    }
    return itemsRead;
}

size_t safe_fwrite(const void *__restrict source, size_t size, size_t count, FILE *__restrict file) {
    size_t itemsWritten = fwrite(source, size, count, file);
    if (itemsWritten < count) {
        fprintf(stderr, "Error writing file\n");
        exit(EXIT_FAILURE);
    }
    return itemsWritten;
}
