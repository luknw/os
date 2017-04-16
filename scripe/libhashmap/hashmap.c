//
// Created by luknw on 4/15/17.
//

#include "hashmap.h"


HashMap *HashMap_new(size_t bucketCount, size_t(*hashcode)(void *key)) {
    HashMap *map = safe_malloc(sizeof(HashMap));

    map->bucketCount = bucketCount;
    map->buckets = safe_calloc(bucketCount, sizeof(LinkedList *));

    map->hashcode = hashcode;

    map->size = 0;

    return map;
}

void HashMap_delete(HashMap *map) {
    for (int i = 0; i < map->bucketCount; ++i) {
        if (map->buckets[i] != NULL) LinkedList_delete(map->buckets[i]);
    }

    safe_free(map);
}

void HashMap_add(HashMap *map, void *key, void *value) {
    size_t hash = map->hashcode(key);

    if (map->buckets[hash] == NULL) map->buckets[hash] = LinkedList_new();

    LinkedList_addFront(map->buckets[hash], value);

    (map->size)++;
}

void *HashMap_remove(HashMap *map, void *key, void *value) {
    size_t hash = map->hashcode(key);
    LinkedList *bucket = map->buckets[hash];

    if (bucket == NULL) return NULL;

    size_t prevBucketSize = bucket->size;
    void *removed = LinkedList_remove(bucket, value);

    if (bucket->size == prevBucketSize) return NULL;

    (map->size)--;

    return removed;
}

bool HashMap_contains(HashMap *map, void *key, void *value) {
    void *newBucketFront = HashMap_remove(map, key, value);

    if (newBucketFront != value) return false;

    HashMap_add(map, key, newBucketFront);

    return true;
}

bool HashMap_isEmpty(HashMap *map) {
    return map->size == 0;
}