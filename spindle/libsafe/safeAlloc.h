//
// Created by luknw on 3/18/17.
//

#ifndef SAFE_ALLOC_H
#define SAFE_ALLOC_H

#include <stdlib.h>
#include <stdio.h>


void *safe_malloc (size_t size);

void *safe_calloc (size_t count, size_t size);

void safe_free (void *ptr);

void *safe_realloc(void *ptr, size_t size);


#endif //SAFE_ALLOC_H
