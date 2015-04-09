/** @file hashtable.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hashtable.h"

// Vyshnav Kakivaya

#define modulo (unsigned int) pow(2, 32)

typedef struct _list_node_t
{
    struct _list_node_t * next;
    struct _list_node_t * prev;
} list_node_t;

typedef struct _list_t
{
    pthread_mutex_t lock;
    list_node_t head;
} list_t;

typedef struct _entry_t
{
    list_node_t list;
    int_hashtable_data_t data;
} entry_t;

list_t* list_create()
{
    list_t* list = malloc(sizeof(list_t));

    pthread_mutex_init(&list->lock, NULL);
    list->head.next = &list->head;
    list->head.prev = &list->head;

    return list;
}

unsigned int get_int_hash(int key)
{
    unsigned int hash = key * 2654435761;
    hash = hash % modulo;

    return hash;
}

unsigned int get_string_hash(char const* key)
{
    int hash = 0;
    while (*key)
    {
        hash = hash * 101 + *key++;
    }

    return hash;
}

int get_slot(unsigned int hash, int depth)
{
    return (hash >> (4 * depth)) & 0xF;
}

void list_destroy(list_t* list)
{
    pthread_mutex_lock(&list->lock);
    list_node_t* current = list->head.next;
    while (current != &list->head)
    {
        entry_t* temp = (entry_t*) current;
        current = current->next;

        free(temp);
    }

    pthread_mutex_unlock(&list->lock);
    pthread_mutex_destroy(&list->lock);

    free(list);

    return;
}

bool list_internal_lookup(list_t* list, int key)
{
    list_node_t* current = list->head.next;
    while (current != &list->head)
    {
        entry_t* current_entry = (entry_t*) current;
        if (key == current_entry->data.key)
        {
            return true;
        }

        current = current->next;
    }

    return false;
}

int list_insert(list_t* list, entry_t* entry)
{
    pthread_mutex_lock(&list->lock);
    bool lookup_data = list_internal_lookup(list, entry->data.key);
    if (lookup_data == false)
    {
        entry->list.next = list->head.next;
        list->head.next->prev = &entry->list;
        entry->list.prev = &list->head;
        list->head.next = &entry->list;

        return entry->data.value;
    }

    pthread_mutex_unlock(&list->lock);
    printf("Key already exists\n");
    return -9999999;
}

entry_t* list_remove(list_t* list, int key)
{
    pthread_mutex_lock(&list->lock);

    entry_t* removed_entry = NULL;
    list_node_t* current = list->head.next;
    while (current != &list->head)
    {
        entry_t* current_entry = (entry_t*) current;
        if (key == current_entry->data.key)
        {
            // It should always be pointing to the head.
            current_entry->list.prev->next = current_entry->list.next;
            current_entry->list.next->prev = current_entry->list.prev;

            removed_entry = current_entry;
            break;
        }
    }

    pthread_mutex_unlock(&list->lock);

    return removed_entry;
}

bool list_lookup(list_t* list, int key)
{
    pthread_mutex_lock(&list->lock);

    bool found_data = list_internal_lookup(list, key);

    pthread_mutex_unlock(&list->lock);

    return found_data;
}

//int_data_t revision_conflict_value = { NULL, NULL, 0 };
//int_data_t* revision_conflict_data = &revision_conflict_value;

int_hashtable_data_t list_update(list_t* list, int_hashtable_data_t* data)
{
    int_hashtable_data_t updated_data = { data->key, data->value, 0};

    pthread_mutex_lock(&list->lock);

    list_node_t* current = list->head.next;
    while (current != &list->head)
    {
        entry_t* current_entry = (entry_t*) current;
        if (data->key == current_entry->data.key)
        {
            if (current_entry->data.rev == data->rev)
            {
                current_entry->data.value = data->value;
                current_entry->data.rev++;
                updated_data.rev = current_entry->data.rev;
                break;
            }

            updated_data.rev = -1;
            break;
        }

        current = current->next;
    }

    pthread_mutex_unlock(&list->lock);

    return updated_data;
}

hashtable_t* internal_hashtable_create(unsigned int size, int depth)
{
    hashtable_t* hash_table = (hashtable_t*) malloc(sizeof(hashtable_t) + sizeof(hashtable_t*) * size);

    pthread_mutex_init(&hash_table->lock, NULL);
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
    pthread_mutex_lock(&hashtable->lock);

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

    pthread_mutex_unlock(&hashtable->lock);

    pthread_mutex_destroy(&hashtable->lock);

    free(hashtable);

    return;
}

int internal_hashtable_insert(hashtable_t* hashtable, entry_t* entry, unsigned int hash, int depth)
{
    int slot = get_slot(hash, depth);

    pthread_mutex_lock(&hashtable->lock);

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

    pthread_mutex_unlock(&hashtable->lock);

    int existing_value = -9999999;
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

int hashtable_insert(hashtable_t* hashtable, int_hashtable_data_t* data)
{
    entry_t* entry = malloc(sizeof(entry_t));
    entry->list.next = NULL;
    entry->list.prev = NULL;
    entry->data.key = data->key;
    entry->data.value = data->value;
    entry->data.rev = data->rev;

    unsigned int hash = get_int_hash(data->key);

    int existing_value = internal_hashtable_insert(hashtable, entry, hash, 0);

    if (existing_value == -9999999)
    {
        free(entry);
    }

    return existing_value;
}

bool internal_hashtable_remove(hashtable_t* hashtable, int key, unsigned int hash, int depth)
{
    int slot = get_slot(hash, depth);

    pthread_mutex_lock(&hashtable->lock);

    void* bucket = hashtable->buckets[slot];

    pthread_mutex_unlock(&hashtable->lock);


    if (bucket == NULL)
        return NULL;

    bool is_removed;
    if (depth < 8)
    {
        is_removed = internal_hashtable_remove(bucket, key, hash, depth + 1);
    }
    else
    {
        entry_t* existing_entry = list_remove(bucket, key);

        if(existing_entry != NULL)
            free(existing_entry);

        is_removed = true;
    }

    return is_removed;
}

bool hashtable_remove(hashtable_t* hashtable, int key)
{
    unsigned int hash = get_int_hash(key);

    return internal_hashtable_remove(hashtable, key, hash, 0);
}

bool internal_hashtable_lookup(hashtable_t* hashtable, int key, unsigned int hash, int depth)
{
    int slot = get_slot(hash, depth);

    pthread_mutex_lock(&hashtable->lock);

    void* bucket = hashtable->buckets[slot];

    pthread_mutex_unlock(&hashtable->lock);

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

bool hashtable_lookup(hashtable_t* hashtable, int key)
{
    unsigned int hash = get_int_hash(key);

    return internal_hashtable_lookup(hashtable, key, hash, 0);
}

int_hashtable_data_t internal_hashtable_update(hashtable_t* hashtable, int_hashtable_data_t* data, unsigned int hash, int depth)
{
    int slot = get_slot(hash, depth);

    pthread_mutex_lock(&hashtable->lock);

    void* bucket = hashtable->buckets[slot];

    pthread_mutex_unlock(&hashtable->lock);

    if (bucket == NULL)
    {
        int_hashtable_data_t ret_val = { data->key, data->value, 0 };
        return ret_val;
    }

    int_hashtable_data_t existing_entry;
    if (depth < 8)
    {
        existing_entry = internal_hashtable_update(bucket, data, hash, depth + 1);
    }
    else
    {
        existing_entry = list_update(bucket, data);
    }

    return existing_entry;
}

int_hashtable_data_t hashtable_update(hashtable_t* hashtable, int_hashtable_data_t* data)
{
    unsigned int hash = get_int_hash(data->key);

    return internal_hashtable_update(hashtable, data, hash, 0);
}

//void list_save(list_t* list, FILE* fd)
//{
//    list_node_t* current = list->head.next;
//    while (current != &list->head)
//    {
//        entry_t* temp = (entry_t*) current;
//        current = current->next;
//
//        fprintf(fd, "%s : %s : %ld\n", temp->data.key, temp->data.value, temp->data.rev);
//
//        free(temp->data.key);
//        free(temp->data.value);
//        free(temp);
//    }
//
//    free(list);
//
//    return;
//}

//void internal_hashtable_save(hashtable_t* hashtable, FILE* fd)
//{
//    int i;
//    if (hashtable->depth < 8)
//    {
//        for (i = 0; i < hashtable->size; i++)
//        {
//            if (hashtable->buckets[i] != NULL)
//                internal_hashtable_save(hashtable->buckets[i], fd);
//        }
//    }
//    else
//    {
//        for (i = 0; i < hashtable->size; i++)
//        {
//            if (hashtable->buckets[i] != NULL)
//                list_save(hashtable->buckets[i], fd);
//        }
//    }
//
//    free(hashtable);
//
//    return;
//}
//
//void hashtable_save(hashtable_t* hashtable)
//{
//    FILE* fd = fopen("./data/table.dat", "w");
//    if (fd == NULL)
//    {
//        perror("fopen failed");
//        exit(-1);
//    }
//    internal_hashtable_save(hashtable, fd);
//
//    fclose(fd);
//
//    return;
//}
//
//void hashtable_load(hashtable_t* hashtable)
//{
//    FILE* fd = fopen("./data/table.dat", "a+");
//    if (fd == NULL)
//    {
//        perror("fopen failed");
//        exit(-1);
//    }
//
//    char key[256], value[256];
//    unsigned long rev;
//
//    do
//    {
//        int count = fscanf(fd, "%s : %s : %ld\n", key, value, &rev);
//        if (count == EOF)
//            break;
//
//        int_data_t data = { key, value, rev };
//        hashtable_insert(hashtable, &data);
//
//    } while (1);
//
//    fclose(fd);
//
//    return;
//}

void list_print(list_t* list)
{
     list_node_t* current = list->head.next;
     while (current != &list->head)
     {
         entry_t* temp = (entry_t*) current;
         current = current->next;
         printf("%d : %d : %d\n", temp->data.key, temp->data.value, temp->data.rev);
     }

     return;
}

void internal_hashtable_print(hashtable_t* hashtable)
{
    if (hashtable->depth < 8)
    {
        for (int i = 0; i < hashtable->size; i++)
        {
            if (hashtable->buckets[i] != NULL)
                internal_hashtable_print(hashtable->buckets[i]);
        }
    }
    else
    {
        for (int i = 0; i < hashtable->size; i++)
        {
            if (hashtable->buckets[i] != NULL)
                list_print(hashtable->buckets[i]);
        }
    }

    return;
}

void hashtable_print(hashtable_t* hashtable)
{
    internal_hashtable_print(hashtable);
}
