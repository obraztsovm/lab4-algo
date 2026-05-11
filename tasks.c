#include "tasks.h"
#include "hash_table/generic.h"
#include "../lab3/vector/generic.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int HashStringPtr(const void *key)
{
    if (!key) return 0;
    const char *str = *(const char**)key;
    if (!str) return 0;
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return (int)(hash & 0x7FFFFFFF);
}

static int CmpStringPtr(const void *a, const void *b)
{
    if (!a || !b) return 0;
    const char *stra = *(const char**)a;
    const char *strb = *(const char**)b;
    if (!stra || !strb) return 0;
    return strcmp(stra, strb) == 0;
}

Vector *removeDuplicates(Vector *strings)
{
    if (!strings || strings->size == 0) {
        Vector *empty = createVector(sizeof(char*));
        return empty;
    }

    HashTable *table = createHashTable(sizeof(char*), sizeof(int));
    Vector *result = createVector(sizeof(char*));

    for (size_t i = 0; i < strings->size; i++) {
        char **str = (char**)getVectorItem(strings, i);
        if (!str || !*str) continue;

        int dummy = 1;
        int *exists = (int*)getItemHashTable(table, str, HashStringPtr, CmpStringPtr);

        if (!exists) {
            setItemHashTable(table, str, &dummy, HashStringPtr, CmpStringPtr);
            appendVectorItem(result, str);
        }
    }

    freeHashTable(table);
    return result;
}

Vector *encodeStrings(Vector *strings)
{
    if (!strings || strings->size == 0) {
        return createVector(sizeof(int));
    }

    HashTable *table = createHashTable(sizeof(char*), sizeof(int));
    Vector *result = createVector(sizeof(int));
    int code = 0;

    for (size_t i = 0; i < strings->size; i++) {
        char **str = (char**)getVectorItem(strings, i);
        if (!str || !*str) {
            int zero = 0;
            appendVectorItem(result, &zero);
            continue;
        }

        int *existing_code = (int*)getItemHashTable(table, str, HashStringPtr, CmpStringPtr);

        if (existing_code) {
            appendVectorItem(result, existing_code);
        } else {
            code++;
            setItemHashTable(table, str, &code, HashStringPtr, CmpStringPtr);
            appendVectorItem(result, &code);
        }
    }

    freeHashTable(table);
    return result;
}

HashTable *swapKeysValues(HashTable *table, HashFunc hash, CmpFunc cmp)
{
    if (!table || !hash || !cmp) {
        return NULL;
    }

    HashTable *new_table = createHashTable(table->val_size, table->key_size);
    if (!new_table) {
        return NULL;
    }

    for (size_t i = 0; i < table->capacity; i++) {
        unsigned char *slot = (unsigned char*)getVectorItem(table->values, i);
        if (!slot || slot[0] != 1) continue;

        void *key = slot + 1;
        void *value = slot + 1 + table->key_size;

        setItemHashTable(new_table, value, key, hash, cmp);
    }

    return new_table;
}