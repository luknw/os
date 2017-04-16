//
// Created by luknw on 4/15/17.
//

#ifndef LIBHASHMAP_H
#define LIBHASHMAP_H

#include <stdlib.h>

#include "../libsafe/safe.h"
#include "../liblinkedlist/linkedList.h"


typedef struct HashMap HashMap;

struct HashMap {
    LinkedList **buckets;

    size_t (*hashcode)(void *key);

    size_t bucketCount;
    size_t size;
};


HashMap *HashMap_new(size_t bucketCount, size_t(*hashcode)(void *key));

void HashMap_delete(HashMap *map);

void HashMap_add(HashMap *map, void *key, void *value);

void *HashMap_remove(HashMap *map, void *key, void *value);

bool HashMap_contains(HashMap *map, void *key, void *value);

bool HashMap_isEmpty(HashMap *map);


#endif //LIBHASHMAP_H
