//
// Created by luknw on 3/18/17.
//

#ifndef IOBENCHMARK_SAFEFILE_H
#define IOBENCHMARK_SAFEFILE_H

#include <stdio.h>
#include <stdlib.h>


FILE *safe_fopen(const char *__restrict filename, const char *__restrict modes);

int safe_fclose(FILE *stream);

int safe_fseek(FILE *stream, long int offset, int whence);

void safe_rewind(FILE *stream);

int safe_fgetpos(FILE *__restrict stream, fpos_t *__restrict position);

int safe_fsetpos(FILE *stream, const fpos_t *position);

size_t safe_fread(void *__restrict target, size_t size, size_t count, FILE *__restrict file);

size_t safe_fwrite(const void *__restrict source, size_t size, size_t count, FILE *__restrict file);


int safe_close(int fd);

__off_t safe_lseek(int fd, __off_t offset, int whence);

ssize_t safe_read(int fd, void *buf, size_t count);

ssize_t safe_write (int fd, const void *buf, size_t count);


#endif //IOBENCHMARK_SAFEFILE_H
