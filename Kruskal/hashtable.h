/** @file hashtable.h */
#pragma once

#include <pthread.h>
#include <stdbool.h>

typedef struct _hashtable_data_t
{
    char* key;
    unsigned long rev;
    int data_size;
    void* value;
} hashtable_data_t;

typedef struct _hashtable_t
{
    int size;
    int depth;

    void* buckets[0];
} hashtable_t;

hashtable_data_t* hashtable_data_create(char const* key, void* value, int rev, int data_size);
void hashtable_data_destroy(hashtable_data_t* data);

hashtable_t* hashtable_create();
void hashtable_destroy(hashtable_t* hashtable);

// Returns the key if inserted. Else, returns NULL.
char* hashtable_insert(hashtable_t* hashtable, hashtable_data_t* data);
bool hashtable_remove(hashtable_t* hashtable, char const* key);
bool hashtable_lookup(hashtable_t* hashtable, char const* key);
void hashtable_update(hashtable_t* hashtable, hashtable_data_t* data);
void* hashtable_find(hashtable_t* hashtable, char const* key);
