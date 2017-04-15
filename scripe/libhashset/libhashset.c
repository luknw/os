//
// Created by luknw on 4/15/17.
//

#include "libhashset.h"


HashSet *HashSet_new(size_t bucketCount, size_t(*hashcode)(void *)) {
    HashSet *set = safe_malloc(sizeof(HashSet));

    set->bucketCount = bucketCount;
    set->buckets = safe_calloc(bucketCount, sizeof(LinkedList *));

    set->hashcode = hashcode;

    set->size = 0;

    return set;
}

void HashSet_delete(HashSet *set) {
    for (int i = 0; i < set->bucketCount; ++i) {
        if (set->buckets[i] != NULL) LinkedList_delete(set->buckets[i]);
    }

    safe_free(set);
}

void HashSet_add(HashSet *set, void *object) {
    size_t hash = set->hashcode(object);

    if (set->buckets[hash] == NULL) set->buckets[hash] = LinkedList_new();

    LinkedList_addFront(set->buckets[hash], object);

    (set->size)++;
}

void *HashSet_remove(HashSet *set, void *object) {
    size_t hash = set->hashcode(object);
    LinkedList *bucket = set->buckets[hash];

    if (bucket == NULL) return NULL;

    size_t prevBucketSize = bucket->size;
    void *removed = LinkedList_remove(bucket, object);

    if (bucket->size == prevBucketSize) return NULL;

    (set->size)--;

    return removed;
}

bool HashSet_contains(HashSet *set, void *object) {
    void *newBucketFront = HashSet_remove(set, object);

    if (newBucketFront != object) return false;

    HashSet_add(set, newBucketFront);

    return true;
}

bool HashSet_isEmpty(HashSet *set) {
    return set->size == 0;
}