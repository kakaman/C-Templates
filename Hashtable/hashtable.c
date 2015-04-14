/** @file hashtable.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hashtable.h"

#define BITS_SIZE 4
#define HASH_BITS ((0x1 << BITS_SIZE) - 1)
#define HASH_SLOTS (HASH_BITS + 1)
#define HASH_DEPTH (((sizeof(int) * 8)/ BITS_SIZE))

#define ALIGN(x) (((x) + __BIGGEST_ALIGNMENT__) & ~(__BIGGEST_ALIGNMENT__ - 1))

#define VALUE(entry) (entry->bytes + ALIGN(entry->key_len))

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
unsigned int hash_function(void const* key, int key_len)
{
    union
    {
        unsigned int int_hash;
        unsigned char char_hash[sizeof(unsigned int)];
    } hash;

    char const* string_key = (char const*) key;
    for(int i = 0; i < 8; i++)
    {
        unsigned char h = table_random_bytes[(string_key[0] + i) % 256];
        for(int j = 1; j < key_len; j++)
        {
            h = table_random_bytes[h ^ string_key[j]];
        }

        hash.char_hash[i] = h;
    }

    return hash.int_hash;
}

int get_slot(unsigned int hash, int depth)
{
    return (hash >> (BITS_SIZE * depth)) & HASH_BITS;
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

        hashtable_data_destroy(temp->data);
        free(temp);
    }
    free(list);

    return;
}

void const* list_internal_find(list_t* list, void const* key, int key_len)
{
    list_node_t* current = list->head.next;
    while (current != &list->head)
    {
        lnode_t* current_entry = (lnode_t*) current;
        if (memcmp(key, current_entry->data->bytes, key_len) == 0)
        {
            // ToDo: Figure out where to access the data.
            return current_entry->data->bytes + sizeof(hashtable_data_t) + ALIGN(current_entry->data->key_len);
            //return VALUE(current_entry->data);
        }

        current = current->next;
    }

    return NULL;
}

bool list_internal_lookup(list_t* list, void const* key, int key_len)
{
    return (list_internal_find(list, key, key_len) != NULL);
}

// Returns the key if inserted. Else, returns NULL.
void const* list_insert(list_t* list, lnode_t* entry)
{
    bool lookup_data = list_internal_lookup(list, entry->data->bytes, entry->data->key_len);
    if (lookup_data == false)
    {
        entry->list.next = list->head.next;
        list->head.next->prev = &entry->list;
        entry->list.prev = &list->head;
        list->head.next = &entry->list;

        return entry->data->bytes;
    }

    return NULL;
}

bool list_remove(list_t* list, void const* key, int key_len)
{
    list_node_t* current = list->head.next;
    while (current != &list->head)
    {
        lnode_t* current_entry = (lnode_t*) current;
        if (memcmp(key, current_entry->data->bytes, key_len) == 0)
        {
            // It should always be pointing to the head.
            current_entry->list.prev->next = current_entry->list.next;
            current_entry->list.next->prev = current_entry->list.prev;

            hashtable_data_destroy(current_entry->data);
            free(current_entry);

            return true;
        }
    }

    return false;
}

void const* list_find(list_t* list, char const* key, int key_len)
{
    return list_internal_find(list, key, key_len);
}

hashtable_data_t* list_update(list_t* list, hashtable_data_t* data)
{
    list_node_t* current = list->head.next;
    while (current != &list->head)
    {
        lnode_t* current_entry = (lnode_t*) current;
        if (memcmp(data->bytes, current_entry->data->bytes, data->key_len) == 0)
        {
            if (current_entry->data->revision == data->revision)
            {
                data->revision++;
                hashtable_data_t* existing_data = current_entry->data;
                current_entry->data = data;

                return existing_data;
            }

            break;
        }

        current = current->next;
    }

    return false;
}


bool list_lookup(list_t* list, char const* key, int key_len)
{
    bool found_data = list_internal_lookup(list, key, key_len);
    return found_data;
}

hashtable_data_t* hashtable_data_create(void const* key, int key_len, void const* value, int value_len, unsigned long long revision)
{
    hashtable_data_t* data = malloc(sizeof(hashtable_data_t) + ALIGN(key_len) + value_len);
    data->key_len = key_len;
    data->value_len = value_len;
    data->revision = revision;

    memcpy(data->bytes, key, key_len);
    memcpy(data->bytes + (sizeof(hashtable_data_t) + ALIGN(key_len)), value, value_len);
    // ToDo: Figure out where to access the data.

    return data;
}

void hashtable_data_destroy(hashtable_data_t* data)
{
    free(data);

    return;
}

hashtable_t* internal_hashtable_create(int depth)
{
    hashtable_t* hash_table = (hashtable_t*) malloc(sizeof(hashtable_t) + sizeof(hashtable_t*) * HASH_SLOTS);

    hash_table->depth = depth;
    memset(hash_table->buckets, 0, sizeof(hashtable_t*) * HASH_SLOTS);

    return hash_table;
}

hashtable_t* hashtable_create()
{
    return internal_hashtable_create(0);
}

void hashtable_destroy(hashtable_t* hashtable)
{
    if (hashtable->depth < HASH_DEPTH)
    {
        for (int i = 0; i < HASH_SLOTS; i++)
        {
            if (hashtable->buckets[i] != NULL)
                hashtable_destroy(hashtable->buckets[i]);
        }
    }
    else
    {
        for (int i = 0; i < HASH_SLOTS; i++)
        {
            if (hashtable->buckets[i] != NULL)
                list_destroy(hashtable->buckets[i]);
        }
    }

    free(hashtable);

    return;
}

void const* internal_hashtable_insert(hashtable_t* hashtable, lnode_t* entry, unsigned int hash, unsigned int depth)
{
    int slot = get_slot(hash, depth);

    void* bucket = hashtable->buckets[slot];
    if (bucket == NULL)
    {
        if (depth < HASH_DEPTH)
        {
            bucket = internal_hashtable_create(depth + 1);
        }
        else
        {
            bucket = list_create();
        }

        hashtable->buckets[slot] = bucket;
    }

    if (depth < HASH_DEPTH)
    {
        return internal_hashtable_insert(bucket, entry, hash, depth + 1);
    }

    return list_insert(bucket, entry);
}

void const* hashtable_insert(hashtable_t* hashtable, void const* key, int key_len, void const* value, int value_len)
{
    lnode_t* entry = malloc(sizeof(lnode_t));
    entry->list.next = NULL;
    entry->list.prev = NULL;
    entry->data = hashtable_data_create(key, key_len, value, value_len, 0);

    unsigned int hash = hash_function(key, key_len);

    void const* existing_value = internal_hashtable_insert(hashtable, entry, hash, 0);
    if (existing_value == NULL)
    {
        hashtable_data_destroy(entry->data);
        free(entry);
    }

    return existing_value;
}

bool internal_hashtable_remove(hashtable_t* hashtable, void const* key, int key_len, unsigned int hash, unsigned int depth)
{
    int slot = get_slot(hash, depth);
    void* bucket = hashtable->buckets[slot];

    if (bucket == NULL)
        return NULL;

    if (depth < HASH_DEPTH)
    {
        return internal_hashtable_remove(bucket, key, key_len, hash, depth + 1);
    }

    return list_remove(bucket, key, key_len);
}

bool hashtable_remove(hashtable_t* hashtable, void const* key, int key_len)
{
    unsigned int hash = hash_function(key, key_len);

    return internal_hashtable_remove(hashtable, key, key_len, hash, 0);
}

bool internal_hashtable_lookup(hashtable_t* hashtable, char const* key, int key_len, unsigned int hash, unsigned int depth)
{
    int slot = get_slot(hash, depth);
    void* bucket = hashtable->buckets[slot];

    if(bucket == NULL)
        return false;

    if (depth < HASH_DEPTH)
    {
        return internal_hashtable_lookup(bucket, key, key_len, hash, depth + 1);
    }

    return list_lookup(bucket, key, key_len);
}

bool hashtable_lookup(hashtable_t* hashtable, void const* key, int key_len)
{
    unsigned int hash = hash_function(key, key_len);

    return internal_hashtable_lookup(hashtable, key, key_len, hash, 0);
}

hashtable_data_t* internal_hashtable_update(hashtable_t* hashtable, hashtable_data_t* data, unsigned int hash, unsigned int depth)
{
    int slot = get_slot(hash, depth);
    void* bucket = hashtable->buckets[slot];

    if (bucket == NULL)
        return NULL;

    if (depth < HASH_DEPTH)
    {
        return internal_hashtable_update(bucket, data, hash, depth + 1);
    }

    return list_update(bucket, data);
}

bool hashtable_update(hashtable_t* hashtable, void const* key, int key_len, void const* value, int value_len, int revision)
{
    unsigned int hash = hash_function(key, key_len);
    hashtable_data_t* data = hashtable_data_create(key, key_len, value, value_len, revision);

    hashtable_data_t* existing_data = internal_hashtable_update(hashtable, data, hash, 0);

    bool found = (existing_data != NULL);
    hashtable_data_destroy(existing_data);

    return found;
}

void const* internal_hashtable_find(hashtable_t* hashtable, void const* key, int key_len, unsigned int hash, unsigned int depth)
{
    int slot = get_slot(hash, depth);
    void* bucket = hashtable->buckets[slot];

    if(bucket == NULL)
        return NULL;

    if (depth < HASH_DEPTH)
    {
        return internal_hashtable_find(bucket, key, key_len, hash, depth + 1);
    }

    return list_find(bucket, key, key_len);
}

void const* hashtable_find(hashtable_t* hashtable, void const* key, int key_len)
{
    unsigned int hash = hash_function(key, key_len);

    return internal_hashtable_find(hashtable, key, key_len, hash, 0);
}
