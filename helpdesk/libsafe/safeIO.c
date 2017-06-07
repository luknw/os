//
// Created by luknw on 3/18/17.
//

#include "safeIO.h"


FILE *safe_fopen(const char *__restrict filename, const char *__restrict modes) {
    FILE *f = fopen(filename, modes);
    if (f == NULL) {
        char *errorString = strerror(errno);
        fprintf(stderr, "Error opening file: %s\n%s\n", filename, errorString);
        exit(EXIT_FAILURE);
    }
    return f;
}

int safe_fclose(FILE *stream) {
    int status = fclose(stream);
    if (status == EOF) {
        perror("Error closing file");
        exit(EXIT_FAILURE);
    }
    return status;
}

int safe_fseek(FILE *stream, long int offset, int whence) {
    int status = fseek(stream, offset, whence);
    if (status == -1) {
        perror("Error operating on file");
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
        perror("Error operating on file");
        exit(EXIT_FAILURE);
    }
    return status;
}

int safe_fsetpos(FILE *stream, const fpos_t *position) {
    int status = fsetpos(stream, position);
    if (status == -1) {
        perror("Error operating on file");
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


int safe_close(int fd) {
    int status = close(fd);
    if (status == -1) {
        perror("Error closing file");
        exit(EXIT_FAILURE);
    }
    return status;
}

void safe_ftruncate(int fd, off_t len) {
    if (ftruncate(fd, len) == -1) {
        perror("Error changing file length");
        exit(EXIT_FAILURE);
    }
}

off_t safe_lseek(int fd, off_t offset, int whence) {
    off_t offsetResult = lseek(fd, offset, whence);
    if (offsetResult == -1) {
        perror("Error operating on file");
        exit(EXIT_FAILURE);
    }
    return offsetResult;
}

ssize_t safe_read(int fd, void *buf, size_t count) {
    ssize_t readCount = read(fd, buf, count);
    if (readCount == -1) {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }// else if (readCount == 0) {
//        fprintf(stderr, "Reached end of file\n");
//    } else if (readCount < count) {
//        fprintf(stderr, "Read less bytes than specified. Expected: %zu Actual: %zi\n", count, readCount);
//    }
    return readCount;
}

ssize_t safe_write(int fd, const void *buf, size_t count) {
    ssize_t writtenCount = write(fd, buf, count);
    if (writtenCount == -1) {
        perror("Error writing file");
        exit(EXIT_FAILURE);
    } else if (writtenCount == 0) {
        fprintf(stderr, "Possible writing error\n");
    } else if (writtenCount < count) {
        fprintf(stderr, "Written less bytes than specified. Expected: %zu Actual: %zi\n", count, writtenCount);
    }
    return writtenCount;
}

int safe_fflush(FILE *stream) {
    int status = fflush(stream);
    if (status == EOF) {
        perror("Error flushing file");
        exit(EXIT_FAILURE);
    }
    return status;
}


ssize_t safe_getline(char **__restrict linePtr, ssize_t *__restrict n, FILE *__restrict stream) {
    *n = getline(linePtr, (size_t *) n, stream);
    if (*n != -1) return *n;
    if (ferror(stream) != 0) {
        perror("Error reading line");
        exit(EXIT_FAILURE);
    }
    if (feof(stream) != 0) return EOF;

    exit(EXIT_FAILURE);
}

ssize_t safe_getline_content(char **__restrict linePtr, ssize_t *__restrict n, FILE *__restrict stream) {
    *n = safe_getline(linePtr, n, stream);
    if (*n > 0 && (*linePtr)[*n - 1] == '\n') {
        (*n)--;
        (*linePtr)[*n] = '\0';
    }
    return *n;
}


FILE *safe_fmemopen(void *pMem, size_t len, const char *modes) {
    FILE *sMem = fmemopen(pMem, len, modes);
    if (sMem == NULL) {
        perror("Error opening memory stream");
        exit(EXIT_FAILURE);
    }
    return sMem;
}


int safe_dup2(int oldFd, int newFd) {
    int duplicate = dup2(oldFd, newFd);
    if (duplicate == -1) {
        perror("Error duplicating file descriptor");
        exit(EXIT_FAILURE);
    }
    return duplicate;
}