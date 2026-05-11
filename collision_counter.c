#include "hash_table/generic.h"
#include "../lab3/vector/generic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1024

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <csv_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: cannot open file %s\n", argv[1]);
        return 1;
    }

    HashTable *table = createHashTable(MAX_LINE_LEN, sizeof(int));
    if (!table) {
        printf("Error: cannot create hash table\n");
        fclose(file);
        return 1;
    }

    char line[MAX_LINE_LEN];
    size_t line_num = 0;

    while (fgets(line, sizeof(line), file)) {
        line_num++;

        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }

        int *count = (int*)getItemHashTable(table, line, HashString, stringEquals);

        if (count) {
            (*count)++;
        } else {
            int value = 1;
            setItemHashTable(table, line, &value, HashString, stringEquals);
        }
    }

    fclose(file);

    unsigned long int collisions = getCollisionCount(table, HashString);

    printf("File: %s\n", argv[1]);
    printf("  Total lines read: %zu\n", line_num);
    printf("  Unique keys: %zu\n", table->size);
    printf("  Capacity: %zu\n", table->capacity);
    printf("  Collisions: %lu\n", collisions);
    printf("  Load factor: %.2f%%\n", (float)table->size / table->capacity * 100);

    freeHashTable(table);

    return 0;
}