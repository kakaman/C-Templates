/*
*  dictionary.h
*
*   Created on: Mar 11, 2015
*       Author: vyshnav
*/

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

// Binary search tree implementation of a Dictionary.

// Dictionary entry struct
typedef struct _dictionary_entry_t
{
    const char* key;
    void* value;

    struct _dictionary_entry_t* left;
    struct _dictionary_entry_t* right;
    struct _dictionary_entry_t* parent;
} dictionary_entry_t;

// Dictionary struct
typedef struct _dictionary_t
{
    dictionary_entry_t* root;
    int size;
} dictionary_t;

// Constructor and Destructor
dictionary_t* dictionary_init();
void dictionary_destroy(dictionary_t* dictionary);
// void dictionary_destroy_all(dictionary_t* dictionary);

// Main Dictionary functions
int dictionary_add(dictionary_t* dictionary, const char* key, void* value);
int dictionary_remove(dictionary_t* dictionary, const char* key);
void* dictionary_get(dictionary_t* dictionary, const char* key);

// Extra functions
int dictionary_parse(dictionary_t* dictionary, char* key_value);

#endif /* DICTIONARY_H_ */
