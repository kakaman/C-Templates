/** @file hashtable.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hashtable.h"

// Vyshnav Kakivaya

// T table taken from http://en.wikipedia.org/wiki/Pearson_hashing
static const unsigned char table_random_bytes[256] =
{
       98,  6, 85,150, 36, 23,112,164,135,207,169,  5, 26, 64,165,219,
       61, 20, 68, 89,130, 63, 52,102, 24,229,132,245, 80,216,195,115,
       90,168,156,203,177,120,  2,190,188,  7,100,185,174,243,162, 10,
      237, 18,253,225,  8,208,172,244,255,126,101, 79,145,235,228,121,
      123,251, 67,250,161,  0,107, 97,241,111,181, 82,249, 33, 69, 55,
       59,153, 29,  9,213,167, 84, 93, 30, 46, 94, 75,151,114, 73,222,
      197, 96,210, 45, 16,227,248,202, 51,152,252,125, 81,206,215,186,
       39,158,178,187,131,136,  1, 49, 50, 17,141, 91, 47,129, 60, 99,
      154, 35, 86,171,105, 34, 38,200,147, 58, 77,118,173,246, 76,254,
      133,232,196,144,198,124, 53,  4,108, 74,223,234,134,230,157,139,
      189,205,199,128,176, 19,211,236,127,192,231, 70,233, 88,146, 44,
      183,201, 22, 83, 13,214,116,109,159, 32, 95,226,140,220, 57, 12,
      221, 31,209,182,143, 92,149,184,148, 62,113, 65, 37, 27,106,166,
        3, 14,204, 72, 21, 41, 56, 66, 28,193, 40,217, 25, 54,179,117,
      238, 87,240,155,180,170,242,212,191,163, 78,218,137,194,175,110,
       43,119,224, 71,122,142, 42,160,104, 48,247,103, 15, 11,138,239
};

// Pearson hash extended to return a 32 bit hash.
unsigned int hash_function(unsigned char* key)
{
    unsigned char hh[8];
    int key_len = strlen(key);
    unsigned int hash = table_random_bytes[key[0]] % 256;
    unsigned char h = 0;
    for(int i = 0; i < 8; i++)
    {
        h = table_random_bytes[key[0] + i] % 256;
        for(int j = 1; j < key_len; j++)
        {
            h = table_random_bytes[h ^ key[j]];
        }

        hh[i] = h;
    }

    for(int i = 0; i < 4; i++)
    {
        hash = hash << (8 * i) | hh[7 - i];
    }

    return hash;
}

// #define modulo (unsigned int) pow(2, 32) ToDo: What is the point of this?

int get_slot(unsigned int hash, int depth)
{
    return (hash >> (4 * depth)) & 0xF;
}

typedef struct _list_node_t
{
    struct _list_node_t * next;
    struct _list_node_t * prev;
} list_node_t;

typedef struct _list_t
{
    list_node_t head;
} list_t;

typedef struct _lnode_t
{
    list_node_t list;
    hashtable_data_t* data;
} lnode_t;

list_t* list_create()
{
    list_t* list = malloc(sizeof(list_t));
    list->head.next = &list->head;
    list->head.prev = &list->head;

    return list;
}

void list_destroy(list_t* list)
{
    list_node_t* current = list->head.next;
    while (current != &list->head)
    {
        lnode_t* temp = (lnode_t*) current;
        current = current->next;

        hashtable_data_destroy(temp->data); // ToDo: Check this? The data is not a ptr.
        free(temp);
    }
    free(list);

    return;
}

bool list_internal_lookup(list_t* list, char const* key)
{
    list_node_t* current = list->head.next;
    while (current != &list->head)
    {
        lnode_t* current_entry = (lnode_t*) current;
        if (strcmp(key, current_entry->data->key) == 0)
        {
            return true;
        }

        current = current->next;
    }

    return false;
}

void* list_internal_find(list_t* list, char const* key)
{
    list_node_t* current = list->head.next;
    while (current != &list->head)
    {
        lnode_t* current_entry = (lnode_t*) current;
        if (strcmp(key, current_entry->data->key) == 0)
        {
            return current_entry->data->value;
        }

        current = current->next;
    }

    return NULL;
}

// Returns the key if inserted. Else, returns NULL.
char* list_insert(list_t* list, lnode_t* entry)
{
    bool lookup_data = list_internal_lookup(list, entry->data->key);
    if (lookup_data == false)
    {
        entry->list.next = list->head.next;
        list->head.next->prev = &entry->list;
        entry->list.prev = &list->head;
        list->head.next = &entry->list;

        return entry->data->key;
    }

    return NULL;
}

lnode_t* list_remove(list_t* list, char const* key)
{
    lnode_t* removed_entry = NULL;
    list_node_t* current = list->head.next;
    while (current != &list->head)
    {
        lnode_t* current_entry = (lnode_t*) current;
        if (strcmp(key, current_entry->data->key) == 0)
        {
            // It should always be pointing to the head.
            current_entry->list.prev->next = current_entry->list.next;
            current_entry->list.next->prev = current_entry->list.prev;

            removed_entry = current_entry;
            break;
        }
    }

    return removed_entry;
}

void* list_find(list_t* list, char const* key)
{
    return list_internal_find(list, key);
}

void list_update(list_t* list, hashtable_data_t* data)
{
    list_node_t* current = list->head.next;
    while (current != &list->head)
    {
        lnode_t* current_entry = (lnode_t*) current;
        if (strcmp(data->key, current_entry->data->key) == 0)
        {
            if (current_entry->data->rev == data->rev)
            {
                memcpy(current_entry->data->value, data->value, data->data_size);
                current_entry->data->rev++;
                break;
            }
            break;
        }

        current = current->next;
    }

    return;
}


bool list_lookup(list_t* list, char const* key)
{
    bool found_data = list_internal_lookup(list, key);
    return found_data;
}

hashtable_data_t* hashtable_data_create(char const* key, void* value, int rev, int data_size)
{
    hashtable_data_t* hashtable_data = malloc(sizeof(hashtable_data_t));
    hashtable_data->key = strdup(key);
    hashtable_data->rev = rev;
    hashtable_data->data_size = data_size;

    hashtable_data->value = malloc(data_size);
    memcpy(hashtable_data->value, value, data_size);

    return hashtable_data;
}

void hashtable_data_destroy(hashtable_data_t* data)
{
    free(data->key);
    free(data->value);
    free(data);

    return;
}

hashtable_t* internal_hashtable_create(unsigned int size, int depth)
{
    hashtable_t* hash_table = (hashtable_t*) malloc(sizeof(hashtable_t) + sizeof(hashtable_t*) * size);

    hash_table->size = size;
    hash_table->depth = depth;
    memset(hash_table->buckets, 0, sizeof(hashtable_t*) * size);

    return hash_table;
}

hashtable_t* hashtable_create()
{
    return internal_hashtable_create(16, 0);
}

void hashtable_destroy(hashtable_t* hashtable)
{
    if (hashtable->depth < 8)
    {
        for (int i = 0; i < hashtable->size; i++)
        {
            if (hashtable->buckets[i] != NULL)
                hashtable_destroy(hashtable->buckets[i]);
        }
    }
    else
    {
        for (int i = 0; i < hashtable->size; i++)
        {
            if (hashtable->buckets[i] != NULL)
                list_destroy(hashtable->buckets[i]);
        }
    }

    free(hashtable);

    return;
}

void* internal_hashtable_insert(hashtable_t* hashtable, lnode_t* entry, unsigned int hash, int depth)
{
    int slot = get_slot(hash, depth);

    void* bucket = hashtable->buckets[slot];
    if (bucket == NULL)
    {
        if (depth < 8)
        {
            bucket = internal_hashtable_create(16, depth + 1);
        }
        else
        {
            bucket = list_create();
        }

        hashtable->buckets[slot] = bucket;
    }

    char* existing_value = NULL;
    if (depth < 8)
    {
        existing_value = internal_hashtable_insert(bucket, entry, hash, depth + 1);
    }
    else
    {
        existing_value = list_insert(bucket, entry);
    }

    return existing_value;
}

char* hashtable_insert(hashtable_t* hashtable, hashtable_data_t* data)
{
    lnode_t* entry = malloc(sizeof(lnode_t));
    entry->list.next = NULL;
    entry->list.prev = NULL;
    entry->data = data;

    unsigned int hash = hash_function((unsigned char*) entry->data->key);

    char* existing_value = internal_hashtable_insert(hashtable, entry, hash, 0);

    if (existing_value == NULL)
    {
        hashtable_data_destroy(entry->data);
        free(entry);
    }

    return existing_value;
}

bool internal_hashtable_remove(hashtable_t* hashtable, char const* key, unsigned int hash, int depth)
{
    int slot = get_slot(hash, depth);
    void* bucket = hashtable->buckets[slot];

    if (bucket == NULL)
        return NULL;

    bool is_removed;
    if (depth < 8)
    {
        is_removed = internal_hashtable_remove(bucket, key, hash, depth + 1);
    }
    else
    {
        lnode_t* existing_entry = list_remove(bucket, key);

        if(existing_entry != NULL)
        {
            hashtable_data_destroy(existing_entry->data);
            free(existing_entry);
        }

        is_removed = true;
    }

    return is_removed;
}

bool hashtable_remove(hashtable_t* hashtable, char const* key)
{
    unsigned int hash = hash_function((unsigned char*) key);

    return internal_hashtable_remove(hashtable, key, hash, 0);
}

bool internal_hashtable_lookup(hashtable_t* hashtable, char const* key, unsigned int hash, int depth)
{
    int slot = get_slot(hash, depth);
    void* bucket = hashtable->buckets[slot];

    if(bucket == NULL)
        return false;

    bool found_entry = false;
    if (depth < 8)
    {
        found_entry = internal_hashtable_lookup(bucket, key, hash, depth + 1);
    }
    else
    {
        found_entry = list_lookup(bucket, key);
    }

    return found_entry;
}

bool hashtable_lookup(hashtable_t* hashtable, char const* key)
{
    unsigned int hash = hash_function((unsigned char*) key);

    return internal_hashtable_lookup(hashtable, key, hash, 0);
}

void internal_hashtable_update(hashtable_t* hashtable, hashtable_data_t* data, unsigned int hash, int depth)
{
    int slot = get_slot(hash, depth);
    void* bucket = hashtable->buckets[slot];

    if (bucket == NULL)
        return;

    if (depth < 8)
    {
        internal_hashtable_update(bucket, data, hash, depth + 1);
    }
    else
    {
        list_update(bucket, data);
    }

    return;
}

void hashtable_update(hashtable_t* hashtable, hashtable_data_t* data)
{
    unsigned int hash = hash_function((unsigned char*) data->key);
    internal_hashtable_update(hashtable, data, hash, 0);
}

void* internal_hashtable_find(hashtable_t* hashtable, char const* key, unsigned int hash, int depth)
{
    int slot = get_slot(hash, depth);
    void* bucket = hashtable->buckets[slot];

    if(bucket == NULL)
        return false;

    void* found_entry = NULL;
    if (depth < 8)
    {
        found_entry = internal_hashtable_find(bucket, key, hash, depth + 1);
    }
    else
    {
        found_entry = list_find(bucket, key);
    }

    return found_entry;
}

void* hashtable_find(hashtable_t* hashtable, char const* key)
{
    unsigned int hash = hash_function((unsigned char*) key);

    return internal_hashtable_find(hashtable, key, hash, 0);
}
