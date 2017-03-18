//
// Created by luknw on 3/18/17.
//

#ifndef IOBENCHMARK_SAFEALLOC_H
#define IOBENCHMARK_SAFEALLOC_H

#include <stdlib.h>


void *safe_malloc (size_t size);

void *safe_calloc (size_t count, size_t size);

void safe_free (void *ptr);


#endif //IOBENCHMARK_SAFEALLOC_H
