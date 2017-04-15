//
// Created by luknw on 4/15/17.
//

#ifndef LIBHASHSET_H
#define LIBHASHSET_H

#include <stdlib.h>

#include "../libsafe/safe.h"
#include "../liblinkedlist/linkedList.h"


typedef struct HashSet HashSet;

struct HashSet {
    LinkedList **buckets;

    size_t (*hashcode)(void *object);

    size_t bucketCount;
    size_t size;
};


HashSet *HashSet_new(size_t maxObjects, size_t(*hashcode)(void *));

void HashSet_delete(HashSet *set);

void HashSet_add(HashSet *set, void *object);

void *HashSet_remove(HashSet *set, void *object);

bool HashSet_contains(HashSet *set, void *object);

bool HashSet_isEmpty(HashSet *set);


#endif //LIBHASHSET_H
