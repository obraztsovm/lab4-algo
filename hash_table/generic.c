#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../../lab3/vector/generic.h"
#include "generic.h"

int HashInt(const void *key)
{
    if (!key) return 0;
    return *(int*)key;
}

int HashString(const void *key)
{
    if (!key) return 0;
    const char *str = (const char*)key;
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return (int)(hash & 0x7FFFFFFF);
}

HashTable *createHashTable(size_t key_size, size_t val_size)
{
    HashTable *table = (HashTable*)malloc(sizeof(HashTable));
    if (!table) {
        return NULL;
    }

    table->key_size = key_size;
    table->val_size = val_size;
    table->size = 0;
    table->capacity = TABLE_MIN_SIZE;

    size_t slot_size = 1 + key_size + val_size;
    table->values = createVector(slot_size);
    if (!table->values) {
        free(table);
        return NULL;
    }

    for (size_t i = 0; i < table->capacity; i++) {
        unsigned char slot[slot_size];
        slot[0] = SLOT_EMPTY;
        memset(slot + 1, 0, key_size + val_size);
        appendVectorItem(table->values, slot);
    }

    return table;
}

static size_t probe(size_t hash, size_t i, size_t capacity)
{
    return (hash + i * i) % capacity;
}

static void* getSlotData(Vector *vec, size_t index)
{
    return getVectorItem(vec, index);
}

static void setSlotStatus(Vector *vec, size_t index, unsigned char status)
{
    unsigned char *slot = (unsigned char*)getVectorItem(vec, index);
    if (slot) {
        slot[0] = status;
    }
}

static unsigned char getSlotStatus(Vector *vec, size_t index)
{
    unsigned char *slot = (unsigned char*)getVectorItem(vec, index);
    if (slot) {
        return slot[0];
    }
    return SLOT_EMPTY;
}

static void* getSlotKey(Vector *vec, size_t index, size_t key_size)
{
    unsigned char *slot = (unsigned char*)getVectorItem(vec, index);
    if (slot && slot[0] == SLOT_OCCUPIED) {
        return slot + 1;
    }
    return NULL;
}

static void* getSlotValue(Vector *vec, size_t index, size_t key_size)
{
    unsigned char *slot = (unsigned char*)getVectorItem(vec, index);
    if (slot && slot[0] == SLOT_OCCUPIED) {
        return slot + 1 + key_size;
    }
    return NULL;
}

static void copyToSlot(Vector *vec, size_t index, void *key, void *value,
                       size_t key_size, size_t val_size)
{
    unsigned char *slot = (unsigned char*)getVectorItem(vec, index);
    if (!slot) return;

    slot[0] = SLOT_OCCUPIED;
    memcpy(slot + 1, key, key_size);
    memcpy(slot + 1 + key_size, value, val_size);
}

void setItemHashTable(HashTable *table, void *key, void *data, HashFunc hash, CmpFunc cmp)
{
    if (!table || !key || !data || !hash || !cmp) {
        return;
    }

    if (table->size * 2 > table->capacity) {
        rehashHashTable(table, hash, cmp);
    }

    int h = hash(key);
    if (h < 0) h = -h;
    size_t hash_val = (size_t)h;

    size_t index = hash_val % table->capacity;
    size_t first_deleted = table->capacity;
    size_t i = 0;

    while (i < table->capacity) {
        size_t current_index = probe(hash_val, i, table->capacity);
        unsigned char status = getSlotStatus(table->values, current_index);

        if (status == SLOT_EMPTY) {
            if (first_deleted != table->capacity) {
                current_index = first_deleted;
            }
            copyToSlot(table->values, current_index, key, data,
                       table->key_size, table->val_size);
            table->size++;
            return;
        }

        if (status == SLOT_DELETED && first_deleted == table->capacity) {
            first_deleted = current_index;
        }

        if (status == SLOT_OCCUPIED) {
            void *existing_key = getSlotKey(table->values, current_index, table->key_size);
            if (existing_key && cmp(existing_key, key) == 1) {
                void *slot_value = getSlotValue(table->values, current_index, table->key_size);
                if (slot_value) {
                    memcpy(slot_value, data, table->val_size);
                }
                return;
            }
        }

        i++;
    }

    if (first_deleted != table->capacity) {
        copyToSlot(table->values, first_deleted, key, data,
                   table->key_size, table->val_size);
        table->size++;
    }
}

void rehashHashTable(HashTable *table, HashFunc hash, CmpFunc cmp)
{
    if (!table || !hash || !cmp) return;

    size_t old_capacity = table->capacity;
    Vector *old_values = table->values;

    size_t new_capacity = table->capacity * 2;
    table->capacity = new_capacity;
    table->size = 0;

    size_t slot_size = 1 + table->key_size + table->val_size;
    table->values = createVector(slot_size);
    if (!table->values) {
        table->values = old_values;
        table->capacity = old_capacity;
        return;
    }

    for (size_t i = 0; i < new_capacity; i++) {
        unsigned char slot[slot_size];
        slot[0] = SLOT_EMPTY;
        memset(slot + 1, 0, table->key_size + table->val_size);
        appendVectorItem(table->values, slot);
    }

    for (size_t i = 0; i < old_capacity; i++) {
        unsigned char status = getSlotStatus(old_values, i);
        if (status == SLOT_OCCUPIED) {
            void *key = getSlotKey(old_values, i, table->key_size);
            void *value = getSlotValue(old_values, i, table->key_size);
            if (key && value) {
                setItemHashTable(table, key, value, hash, cmp);
            }
        }
    }

    free(old_values);
}

void *getItemHashTable(HashTable *table, void *key, HashFunc hash, CmpFunc cmp)
{
    if (!table || !key || !hash || !cmp) {
        return NULL;
    }

    int h = hash(key);
    if (h < 0) h = -h;
    size_t hash_val = (size_t)h;

    size_t i = 0;
    while (i < table->capacity) {
        size_t index = probe(hash_val, i, table->capacity);
        unsigned char status = getSlotStatus(table->values, index);

        if (status == SLOT_EMPTY) {
            return NULL;
        }

        if (status == SLOT_OCCUPIED) {
            void *existing_key = getSlotKey(table->values, index, table->key_size);
            if (existing_key && cmp(existing_key, key) == 1) {
                return getSlotValue(table->values, index, table->key_size);
            }
        }

        i++;
    }

    return NULL;
}

void *popItemHashTable(HashTable *table, void *key, HashFunc hash, CmpFunc cmp)
{
    if (!table || !key || !hash || !cmp) {
        return NULL;
    }

    int h = hash(key);
    if (h < 0) h = -h;
    size_t hash_val = (size_t)h;

    size_t i = 0;
    while (i < table->capacity) {
        size_t index = probe(hash_val, i, table->capacity);
        unsigned char status = getSlotStatus(table->values, index);

        if (status == SLOT_EMPTY) {
            return NULL;
        }

        if (status == SLOT_OCCUPIED) {
            void *existing_key = getSlotKey(table->values, index, table->key_size);
            if (existing_key && cmp(existing_key, key) == 1) {
                void *value = getSlotValue(table->values, index, table->key_size);
                void *result = malloc(table->val_size);
                if (result && value) {
                    memcpy(result, value, table->val_size);
                }
                setSlotStatus(table->values, index, SLOT_DELETED);
                table->size--;
                return result;
            }
        }

        i++;
    }

    return NULL;
}

unsigned long int getCollisionCount(HashTable *table, HashFunc hash)
{
    if (!table || !hash) {
        return 0;
    }

    unsigned long int collisions = 0;

    for (size_t i = 0; i < table->capacity; i++) {
        unsigned char status = getSlotStatus(table->values, i);
        if (status == SLOT_OCCUPIED) {
            void *key = getSlotKey(table->values, i, table->key_size);
            if (key) {
                int h = hash(key);
                if (h < 0) h = -h;
                size_t ideal_index = (size_t)h % table->capacity;
                if (ideal_index != i) {
                    collisions++;
                }
            }
        }
    }

    return collisions;
}

void freeHashTable(HashTable *table)
{
    if (!table) return;

    if (table->values) {
        vectorFree(table->values);
    }

    free(table);
}