//
// Created by luknw on 3/18/17.
//

#ifndef SAFE_IO_H
#define SAFE_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


FILE *safe_fopen(const char *__restrict filename, const char *__restrict modes);

int safe_fclose(FILE *stream);

int safe_fseek(FILE *stream, long int offset, int whence);

void safe_rewind(FILE *stream);

int safe_fgetpos(FILE *__restrict stream, fpos_t *__restrict position);

int safe_fsetpos(FILE *stream, const fpos_t *position);

size_t safe_fread(void *__restrict target, size_t size, size_t count, FILE *__restrict file);

size_t safe_fwrite(const void *__restrict source, size_t size, size_t count, FILE *__restrict file);


#define safe_open(fdResult, file, ...) \
            fdResult = open(argv[1], O_RDONLY); \
            if(fdResult == -1) { \
            perror("Error opening file"); \
            exit(EXIT_FAILURE); \
            }

int safe_close(int fd);

__off_t safe_lseek(int fd, __off_t offset, int whence);

ssize_t safe_read(int fd, void *buf, size_t count);

ssize_t safe_write(int fd, const void *buf, size_t count);


ssize_t safe_getline(char **__restrict linePtr, ssize_t *__restrict n, FILE *__restrict stream);

ssize_t safe_getline_content(char **__restrict linePtr, ssize_t *__restrict n, FILE *__restrict stream);


FILE *safe_fmemopen (void *pMem, size_t len, const char *modes);


#endif //SAFE_IO_H
