#include "tasks.h"
#include "../lab3/vector/generic.h"
#include "hash_table/generic.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

static void test_remove_duplicates()
{
    printf("Test 1: removeDuplicates ... ");

    Vector *vec = createVector(sizeof(char*));
    const char *words[] = {"banana", "mama", "banana", "papa", "mama", "apple"};

    for (int i = 0; i < 6; i++) {
        appendVectorItem(vec, &words[i]);
    }

    Vector *unique = removeDuplicates(vec);

    assert(unique->size == 4);

    const char *expected[] = {"banana", "mama", "papa", "apple"};
    for (size_t i = 0; i < unique->size; i++) {
        char **str = (char**)getVectorItem(unique, i);
        assert(strcmp(*str, expected[i]) == 0);
    }

    vectorFree(vec);
    vectorFree(unique);
    printf("OK\n");
}

static void test_encode_strings()
{
    printf("Test 2: encodeStrings ... ");

    Vector *vec = createVector(sizeof(char*));
    const char *words[] = {"apple", "banana", "apple", "cherry", "banana", "apple"};

    for (int i = 0; i < 6; i++) {
        appendVectorItem(vec, &words[i]);
    }

    Vector *encoded = encodeStrings(vec);

    assert(encoded->size == 6);

    int *codes = (int*)malloc(encoded->size * sizeof(int));
    for (size_t i = 0; i < encoded->size; i++) {
        codes[i] = *(int*)getVectorItem(encoded, i);
    }

    assert(codes[0] == codes[2]);
    assert(codes[0] == codes[5]);
    assert(codes[1] == codes[4]);
    assert(codes[0] != codes[1]);
    assert(codes[2] != codes[3]);
    assert(codes[3] != codes[4]);

    free(codes);
    vectorFree(vec);
    vectorFree(encoded);
    printf("OK\n");
}

static void test_swap_keys_values()
{
    printf("Test 3: swapKeysValues ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));

    int keys[] = {1, 2, 3, 4, 5};
    int values[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < 5; i++) {
        setItemHashTable(table, &keys[i], &values[i], HashInt, intEquals);
    }

    HashTable *swapped = swapKeysValues(table, HashInt, intEquals);

    assert(swapped != NULL);
    assert(swapped->size == 5);

    for (int i = 0; i < 5; i++) {
        int *val = (int*)getItemHashTable(swapped, &values[i], HashInt, intEquals);
        assert(val != NULL && *val == keys[i]);
    }

    freeHashTable(table);
    freeHashTable(swapped);
    printf("OK\n");
}

static void test_swap_keys_values_empty()
{
    printf("Test 4: swapKeysValues on empty table ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));
    HashTable *swapped = swapKeysValues(table, HashInt, intEquals);

    assert(swapped != NULL);
    assert(swapped->size == 0);

    freeHashTable(table);
    freeHashTable(swapped);
    printf("OK\n");
}

static void test_remove_duplicates_empty()
{
    printf("Test 5: removeDuplicates empty vector ... ");

    Vector *vec = createVector(sizeof(char*));
    Vector *unique = removeDuplicates(vec);

    assert(unique != NULL);
    assert(unique->size == 0);

    vectorFree(vec);
    vectorFree(unique);
    printf("OK\n");
}

static void test_encode_strings_empty()
{
    printf("Test 6: encodeStrings empty vector ... ");

    Vector *vec = createVector(sizeof(char*));
    Vector *encoded = encodeStrings(vec);

    assert(encoded != NULL);
    assert(encoded->size == 0);

    vectorFree(vec);
    vectorFree(encoded);
    printf("OK\n");
}

int main()
{
    printf("\nTasks Tests\n\n");

    test_remove_duplicates();
    test_encode_strings();
    test_swap_keys_values();
    test_swap_keys_values_empty();
    test_remove_duplicates_empty();
    test_encode_strings_empty();

    printf("\nAll tests passed \n\n");
    return 0;
}