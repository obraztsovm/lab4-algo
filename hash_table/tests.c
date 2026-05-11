#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "generic.h"
#include "../../lab3/comparators.h"

static void test_create_table()
{
    printf("Test 1: createHashTable ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));
    assert(table != NULL);
    assert(table->size == 0);
    assert(table->capacity == TABLE_MIN_SIZE);
    assert(table->key_size == sizeof(int));
    assert(table->val_size == sizeof(int));
    assert(table->values != NULL);

    freeHashTable(table);
    printf("OK\n");
}

static void test_insert_and_get()
{
    printf("Test 2: insert and get ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));
    int key1 = 10, val1 = 100;
    int key2 = 20, val2 = 200;
    int key3 = 30, val3 = 300;

    setItemHashTable(table, &key1, &val1, HashInt, intEquals);
    setItemHashTable(table, &key2, &val2, HashInt, intEquals);
    setItemHashTable(table, &key3, &val3, HashInt, intEquals);

    int *res1 = (int*)getItemHashTable(table, &key1, HashInt, intEquals);
    int *res2 = (int*)getItemHashTable(table, &key2, HashInt, intEquals);
    int *res3 = (int*)getItemHashTable(table, &key3, HashInt, intEquals);

    assert(res1 != NULL && *res1 == 100);
    assert(res2 != NULL && *res2 == 200);
    assert(res3 != NULL && *res3 == 300);
    assert(table->size == 3);

    freeHashTable(table);
    printf("OK\n");
}

static void test_update_value()
{
    printf("Test 3: update existing key ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));
    int key = 42, val1 = 100, val2 = 200;

    setItemHashTable(table, &key, &val1, HashInt, intEquals);
    int *res = (int*)getItemHashTable(table, &key, HashInt, intEquals);
    assert(res != NULL && *res == 100);

    setItemHashTable(table, &key, &val2, HashInt, intEquals);
    res = (int*)getItemHashTable(table, &key, HashInt, intEquals);
    assert(res != NULL && *res == 200);
    assert(table->size == 1);

    freeHashTable(table);
    printf("OK\n");
}

static void test_string_keys()
{
    printf("Test 4: string keys ... ");

    HashTable *table = createHashTable(100, sizeof(int));

    char key1[] = "apple";
    char key2[] = "banana";
    char key3[] = "cherry";
    int val1 = 10, val2 = 20, val3 = 30;

    setItemHashTable(table, key1, &val1, HashString, stringEquals);
    setItemHashTable(table, key2, &val2, HashString, stringEquals);
    setItemHashTable(table, key3, &val3, HashString, stringEquals);

    int *res1 = (int*)getItemHashTable(table, key1, HashString, stringEquals);
    int *res2 = (int*)getItemHashTable(table, key2, HashString, stringEquals);
    int *res3 = (int*)getItemHashTable(table, key3, HashString, stringEquals);

    assert(res1 != NULL && *res1 == 10);
    assert(res2 != NULL && *res2 == 20);
    assert(res3 != NULL && *res3 == 30);
    assert(table->size == 3);

    freeHashTable(table);
    printf("OK\n");
}

static void test_collisions()
{
    printf("Test 5: collisions handling ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));

    int keys[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    for (int i = 0; i < 10; i++) {
        setItemHashTable(table, &keys[i], &keys[i], HashInt, intEquals);
    }

    for (int i = 0; i < 10; i++) {
        int *res = (int*)getItemHashTable(table, &keys[i], HashInt, intEquals);
        assert(res != NULL && *res == keys[i]);
    }

    freeHashTable(table);
    printf("OK\n");
}

static void test_rehash()
{
    printf("Test 6: rehash on load factor > 0.5 ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));
    size_t initial_capacity = table->capacity;

    for (int i = 0; i < initial_capacity; i++) {
        setItemHashTable(table, &i, &i, HashInt, intEquals);
    }

    assert(table->size == initial_capacity);
    assert(table->capacity > initial_capacity);

    for (int i = 0; i < initial_capacity; i++) {
        int *res = (int*)getItemHashTable(table, &i, HashInt, intEquals);
        assert(res != NULL && *res == i);
    }

    freeHashTable(table);
    printf("OK\n");
}

static void test_delete()
{
    printf("Test 7: delete item ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));
    int key = 99, val = 999;

    setItemHashTable(table, &key, &val, HashInt, intEquals);
    assert(table->size == 1);

    int *popped = (int*)popItemHashTable(table, &key, HashInt, intEquals);
    assert(popped != NULL && *popped == 999);
    free(popped);

    assert(table->size == 0);

    int *res = (int*)getItemHashTable(table, &key, HashInt, intEquals);
    assert(res == NULL);

    freeHashTable(table);
    printf("OK\n");
}

static void test_reinsert_after_delete()
{
    printf("Test 8: reinsert after delete ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));
    int key = 77, val1 = 777, val2 = 888;

    setItemHashTable(table, &key, &val1, HashInt, intEquals);
    int *popped = (int*)popItemHashTable(table, &key, HashInt, intEquals);
    assert(popped != NULL && *popped == 777);
    free(popped);

    setItemHashTable(table, &key, &val2, HashInt, intEquals);
    int *res = (int*)getItemHashTable(table, &key, HashInt, intEquals);
    assert(res != NULL && *res == 888);
    assert(table->size == 1);

    freeHashTable(table);
    printf("OK\n");
}

static void test_collision_count()
{
    printf("Test 9: collision count ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));

    for (int i = 0; i < 5; i++) {
        setItemHashTable(table, &i, &i, HashInt, intEquals);
    }

    unsigned long int collisions = getCollisionCount(table, HashInt);

    freeHashTable(table);
    printf("collisions=%lu OK\n", collisions);
}

static void test_empty_table()
{
    printf("Test 10: empty table operations ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));
    int key = 123;

    int *res = (int*)getItemHashTable(table, &key, HashInt, intEquals);
    assert(res == NULL);

    int *popped = (int*)popItemHashTable(table, &key, HashInt, intEquals);
    assert(popped == NULL);

    assert(table->size == 0);

    freeHashTable(table);
    printf("OK\n");
}

static void test_null_handling()
{
    printf("Test 11: NULL handling ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));
    int key = 1, val = 2;

    setItemHashTable(NULL, &key, &val, HashInt, intEquals);
    setItemHashTable(table, NULL, &val, HashInt, intEquals);
    setItemHashTable(table, &key, NULL, HashInt, intEquals);
    setItemHashTable(table, &key, &val, NULL, intEquals);
    setItemHashTable(table, &key, &val, HashInt, NULL);

    void *res = getItemHashTable(NULL, &key, HashInt, intEquals);
    assert(res == NULL);

    void *popped = popItemHashTable(NULL, &key, HashInt, intEquals);
    assert(popped == NULL);

    freeHashTable(NULL);

    freeHashTable(table);
    printf("OK\n");
}

static void test_get_collision_count_empty()
{
    printf("Test 12: collision count on empty table ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));
    unsigned long int collisions = getCollisionCount(table, HashInt);
    assert(collisions == 0);

    freeHashTable(table);
    printf("OK\n");
}

static void test_large_data()
{
    printf("Test 13: large data insertion ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));

    for (int i = 0; i < 1000; i++) {
        setItemHashTable(table, &i, &i, HashInt, intEquals);
    }

    assert(table->size == 1000);

    int missing = 2000;
    int *res = (int*)getItemHashTable(table, &missing, HashInt, intEquals);
    assert(res == NULL);

    for (int i = 0; i < 1000; i++) {
        int *val = (int*)getItemHashTable(table, &i, HashInt, intEquals);
        assert(val != NULL && *val == i);
    }

    freeHashTable(table);
    printf("OK\n");
}

static void test_pop_all_elements()
{
    printf("Test 14: pop all elements ... ");

    HashTable *table = createHashTable(sizeof(int), sizeof(int));
    int n = 50;

    for (int i = 0; i < n; i++) {
        setItemHashTable(table, &i, &i, HashInt, intEquals);
    }

    for (int i = 0; i < n; i++) {
        int *popped = (int*)popItemHashTable(table, &i, HashInt, intEquals);
        assert(popped != NULL && *popped == i);
        free(popped);
    }

    assert(table->size == 0);

    freeHashTable(table);
    printf("OK\n");
}

int main()
{
    printf("\n HashTable Tests \n\n");

    test_create_table();
    test_insert_and_get();
    test_update_value();
    test_string_keys();
    test_collisions();
    test_rehash();
    test_delete();
    test_reinsert_after_delete();
    test_collision_count();
    test_empty_table();
    test_null_handling();
    test_get_collision_count_empty();
    test_large_data();
    test_pop_all_elements();

    printf("\n All tests passed \n\n");
    return 0;
}