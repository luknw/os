//
// Created by luknw on 3/18/17.
//

#ifndef IOBENCHMARK_SAFEFILE_H
#define IOBENCHMARK_SAFEFILE_H

#include <stdio.h>
#include <stdlib.h>


FILE *safe_fopen(const char *__restrict __filename, const char *__restrict __modes);

int safe_fclose(FILE *__stream);

size_t safe_fread(void *__restrict target, size_t size, size_t count, FILE *__restrict file);

size_t safe_fwrite(const void *__restrict source, size_t size, size_t count, FILE *__restrict file);


#endif //IOBENCHMARK_SAFEFILE_H
