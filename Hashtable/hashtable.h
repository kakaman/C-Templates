/** @file hashtable.h */
#ifndef __HT_H__
#define __HT_H__
#include <pthread.h>
#include <stdbool.h>

typedef struct _int_data_t
{
    int key;
    int value;
    int rev;
} int_data_t;

//typedef struct _char_data_t
//{
//    char* key;
//    char* value;
//    unsigned long rev;
//} char_data_t;

typedef struct _hashtable_t
{
    int size;
    int depth;

    void* buckets[0];
} hashtable_t;

hashtable_t* hashtable_create();
void hashtable_destroy(hashtable_t* hashtable);
void hashtable_load();
void hashtable_save();

int hashtable_insert(hashtable_t* hashtable, int_data_t* data);
bool hashtable_remove(hashtable_t* hashtable, int key);
bool hashtable_lookup(hashtable_t* hashtable, int key);
int_data_t hashtable_update(hashtable_t* hashtable, int_data_t* data);

// Hash functions
unsigned int get_string_hash(char const* key);
unsigned int get_int_hash(int key);

// Data is variable
hashtable_t* hashtable_init(int data_size, void (*print)(void const*));
void clear_hashtable(hashtable_t* hashtable);

// Internal functions
int get_size(hashtable_t* hashtable);

#endif
