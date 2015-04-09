#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "hashtable.h"

static hashtable_t* hashtable;

int add(int key, int value)
{
    int_hashtable_data_t data = {key, value, 0};
    printf("Adding Key: %d, Value: %d\n", key, value);

    return hashtable_insert(hashtable, &data);
}

bool remove_key(int key)
{
    printf("Removing Key: %d, Value: %d\n", key, key);
    return hashtable_remove(hashtable, key);
}

bool lookup(int key)
{
    printf("Lookup Key: %d, Value: %d\n", key, key);
    return hashtable_lookup(hashtable, key);
}

void print_data(int_hashtable_data_t* data, bool stream)
{
    if(!stream)
    {
        printf("Key: %d, Value: %d\n", data->key, data->value);
    }

    else
    {
        printf("(Key: %d, Value: %d) ", data->key, data->value);
    }

    return;
}

int main()
{
    hashtable = hashtable_create();

    add(1,1);
    add(2,2);
    add(3,3);
    bool found = lookup(1);
    printf("\nFound: %d\n", found);
    found = lookup(3);
    printf("Found: %d\n", found);
    found = lookup(4);
    printf("Found: %d\n", found);

    return 1;
}
