/*
*  dictionary.h
*
*   Created on: Mar 11, 2015
*       Author: vyshnav
*/

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include <pthread.h>

#define KEY_EXISTS 1
#define NO_KEY_EXISTS 2
#define ILLEGAL_FORMAT 3

// Thread safe Binary search tree implementation of a Dictionary.

// Dictionary entry struct
typedef struct _dictionary_entry_t
{
    pthread_mutex_t lock;
    const char* key;
    void* value;

    dictionary_entry_t* left;
    dictionary_entry_t* right;
    // dictionary_entry_t* parent;
} dictionary_entry_t;

// Dictionary struct
typedef struct _dictionary_t
{
    pthread_mutex_t mutex;
    dictionary_entry_t* root;
} dictionary_t;

// Constructor and Destructor
void dictionary_init(dictionary_t* dictionary);
void dictionary_destroy(dictionary_t* dictionary);
void dictionary_destroy_all(dictionary_t* dictionary);

// Main Dictionary functions
int dictionary_add(dictionary_t* dictionary, const char* key, const char* value);
int dictionary_remove(dictionary_t* dictionary, const char* key);
const char* dictionary_get(dictionary_t* dictionary, const char* key);

// Extra functions
int dictionary_parse(dictionary_t* dictionary, char* key_value);

#endif /* DICTIONARY_H_ */
