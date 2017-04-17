//
// Created by luknw on 4/15/17.
//

#include "hashmap.h"


typedef struct HashMapEntry HashMapEntry;

static struct HashMapEntry {
    void *key;
    void *value;
};

static HashMapEntry *HashMapEntry_new(void *key, void *value) {
    HashMapEntry *entry = safe_malloc(sizeof(HashMapEntry));

    entry->key = key;
    entry->value = value;

    return entry;
}

static void HashMapEntry_delete(HashMapEntry *entry) {
    safe_free(entry);
}


static size_t HashMap_hash(HashMap *map, void *key) {
    size_t hash = map->hashcode(key);
    hash ^= hash >> (sizeof(size_t) * 8 / 2);
    return (map->bucketCount - 1) & hash;
}

HashMap *HashMap_new(size_t(*hashcode)(void *key)) {
    HashMap *map = safe_malloc(sizeof(HashMap));

    map->bucketCount = HashMap_DEFAULT_BUCKET_COUNT;
    map->buckets = safe_calloc(map->bucketCount, sizeof(LinkedList *));

    map->hashcode = hashcode;

    map->size = 0;

    return map;
}

void HashMap_delete(HashMap *map) {
    for (LinkedList **bucket = map->buckets; map->bucketCount-- > 0; ++bucket) {
        if (*bucket == NULL) continue;

        while (!LinkedList_isEmpty(*bucket)) {
            HashMapEntry_delete(LinkedList_removeBack(*bucket));
        }
        LinkedList_delete(*bucket);
    }

    safe_free(map);
}

void HashMap_add(HashMap *map, void *key, void *value) {
    size_t hash = HashMap_hash(map, key);

    if (map->buckets[hash] == NULL) map->buckets[hash] = LinkedList_new();

    LinkedList_addFront(map->buckets[hash], HashMapEntry_new(key, value));

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
    HashMapEntry *removed = LinkedList_remove(bucket, entry);

    if (removed == NULL) return NULL;

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