//
// Created by luknw on 4/15/17.
//

#include "hashMap.h"


static HashMapEntry *HashMapEntry_new(void *key, void *value, HashMapEntry *prev, HashMapEntry *next) {
    HashMapEntry *entry = safe_malloc(sizeof(HashMapEntry));

    entry->key = key;
    entry->value = value;

    entry->prev = prev;
    entry->next = next;

    return entry;
}

static void HashMapEntry_delete(HashMapEntry *entry) {
    if (entry->prev != NULL) entry->prev->next = entry->next;
    if (entry->next != NULL) entry->next->prev = entry->prev;

    safe_free(entry);
}


static size_t HashMap_hash(HashMap *map, void *key) {
    size_t hash = map->hashcode(key);
    hash ^= hash >> (sizeof(size_t) * CHAR_BIT / 2);
    return (map->bucketCount - 1) & hash;
}

HashMap *HashMap_new(size_t(*hashcode)(void *key)) {
    HashMap *map = safe_malloc(sizeof(HashMap));

    map->bucketCount = HashMap_DEFAULT_BUCKET_COUNT;
    map->buckets = safe_calloc(map->bucketCount, sizeof(LinkedList *));

    map->entries = HashMapEntry_new(NULL, NULL, NULL, NULL);
    map->entries->prev = map->entries->next = map->entries;

    map->hashcode = hashcode;

    map->size = 0;

    return map;
}

void HashMap_delete(HashMap *map) {
    for (LinkedList **bucket = map->buckets; map->bucketCount-- > 0; ++bucket) {
        if (*bucket != NULL) LinkedList_delete(*bucket);
    }

    while (map->entries->next != map->entries) {
        HashMapEntry_delete(map->entries->next);
    }
    HashMapEntry_delete(map->entries);

    safe_free(map);
}

void HashMap_add(HashMap *map, void *key, void *value) {
    size_t hash = HashMap_hash(map, key);

    if (map->buckets[hash] == NULL) map->buckets[hash] = LinkedList_new();

    HashMapEntry *newEntry = HashMapEntry_new(key, value, map->entries->prev, map->entries);
    map->entries->prev->next = newEntry;
    map->entries->prev = newEntry;

    LinkedList_addFront(map->buckets[hash], newEntry);

    (map->size)++;
}

/// Convenient encapsulation breaking solution :p
static HashMapEntry *HashMap_findEntryInBucket(LinkedList *bucket, void *key) {
    for (LinkedListNode *node = bucket->front->next; node != bucket->back; node = node->next) {
        HashMapEntry *entry = node->object;
        if (entry->key == key) return entry;
    }

    return NULL;
}

void *HashMap_remove(HashMap *map, void *key) {
    size_t hash = HashMap_hash(map, key);
    LinkedList *bucket = map->buckets[hash];

    if (bucket == NULL) return NULL;

    HashMapEntry *entry = HashMap_findEntryInBucket(bucket, key);
    if (entry == NULL) return NULL;

    HashMapEntry *removed = LinkedList_remove(bucket, entry);

    void *removedValue = removed->value;
    HashMapEntry_delete(removed);

    (map->size)--;

    return removedValue;
}

bool HashMap_contains(HashMap *map, void *key) {
    size_t initialMapSize = map->size;

    void *newBucketFront = HashMap_remove(map, key);

    if (map->size == initialMapSize) return false;

    HashMap_add(map, key, newBucketFront);

    return true;
}

void *HashMap_get(HashMap *map, void *key) {
    size_t initialMapSize = map->size;

    void *newBucketFront = HashMap_remove(map, key);

    if (map->size != initialMapSize) HashMap_add(map, key, newBucketFront);

    return newBucketFront;
}

bool HashMap_isEmpty(HashMap *map) {
    return map->size == 0;
}