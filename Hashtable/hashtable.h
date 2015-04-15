/** @file hashtable.h */
#pragma once

#include <pthread.h>
#include <stdbool.h>

typedef struct _hashtable_data_t
{
    int key_len;
    int value_len;
    unsigned long long revision;
    unsigned char bytes[0] __attribute__((aligned));
} hashtable_data_t;

typedef struct _hashtable_t
{
    unsigned int depth;

    void* buckets[0] __attribute__((aligned));
} hashtable_t;

hashtable_data_t* hashtable_data_create(void const* key, int key_len, void const* value, int value_len, unsigned long long revision);
void hashtable_data_destroy(hashtable_data_t* data);

hashtable_t* hashtable_create();
void hashtable_destroy(hashtable_t* hashtable);

// Returns the key if inserted. Else, returns NULL.
void const* hashtable_insert(hashtable_t* hashtable, void const* key, int key_len, void const* value, int value_len);
bool hashtable_remove(hashtable_t* hashtable, void const* key, int key_len);
bool hashtable_lookup(hashtable_t* hashtable, void const* key, int key_len);
bool hashtable_update(hashtable_t* hashtable, void const* key, int key_len, void const* value, int value_len, int revision);
void const* hashtable_find(hashtable_t* hashtable, void const* key, int key_len);
