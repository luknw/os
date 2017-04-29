//
// Created by luknw on 4/15/17.
//

#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <limits.h>

#include "../libsafe/safe.h"
#include "../liblinkedlist/linkedList.h"


typedef struct HashMapEntry HashMapEntry;
typedef struct HashMap HashMap;


struct HashMapEntry {
    void *key;
    void *value;

    HashMapEntry *prev;
    HashMapEntry *next;
};

struct HashMap {
    LinkedList **buckets;
    HashMapEntry *entries;

    size_t (*hashcode)(void *key);

    size_t bucketCount;
    size_t size;
};


/// Must be power of 2
static const size_t HashMap_DEFAULT_BUCKET_COUNT = 32;


HashMap *HashMap_new(size_t(*hashcode)(void *key));

void HashMap_delete(HashMap *map);

void HashMap_add(HashMap *map, void *key, void *value);

void *HashMap_remove(HashMap *map, void *key);

bool HashMap_contains(HashMap *map, void *key);

void *HashMap_get(HashMap *map, void *key);

bool HashMap_isEmpty(HashMap *map);


#endif //HASHMAP_H
