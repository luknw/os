//
// Created by luknw on 3/18/17.
//

#ifndef IOBENCHMARK_SAFEALLOC_H
#define IOBENCHMARK_SAFEALLOC_H

#include <stdlib.h>


void *safe_calloc (size_t __nmemb, size_t __size);

void safe_free (void *ptr);


#endif //IOBENCHMARK_SAFEALLOC_H
