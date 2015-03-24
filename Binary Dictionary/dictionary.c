/*
 * dictionary.c
 *
 *  Created on: Mar 11, 2015
 *      Author: vyshnav
 */

#include <search.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"

dictionary_entry_t* entry_init(const char* key, void* element)
{
    dictionary_entry_t* entry = malloc(sizeof(dictionary_entry_t));

    entry->key = key;
    entry->value = element;

    entry->left = NULL;
    entry->right = NULL;
    entry->parent = NULL;

    return entry;
}

dictionary_t* dictionary_init()
{
    dictionary_t* dictionary = malloc(sizeof(dictionary_t));

    dictionary->root = NULL;
    dictionary->size = 0;

    return dictionary;
}

// Comparison function. Returns -1 if invalid, 1 if smaller, 2 if greater, and 0 if same.
int compare(const char* left, const char* right)
{
    if(left == NULL || right == NULL)
    {
        return -1;
    }
    else if(atoi(left) < atoi(right))
    {
        return 1;
    }
    else if(atoi(left) > atoi(right))
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

void dictionary_destroy(dictionary_t* dictionary)
{
    return;
}

// Todo: Make sure the pointers are actually being changed
void swap(dictionary_entry_t** left, dictionary_entry_t** right)
{
    if((left == NULL) || (right == NULL))
    {
        return;
    }

    dictionary_entry_t* temp = *left;
    *left = *right;
    *right = temp;

    temp = NULL;

    return;
}

// Find element in dictionary. Returns ptr to entry, returns NULL if entry DNE.
dictionary_entry_t* find(dictionary_entry_t* current, const char* key)
{
    if((current == NULL) || (key == NULL))
    {
        return NULL;
    }

    int comp = compare(current->key, key);

    if(comp == 0)
    {
        return current;
    }
    else if(comp == 1)
    {
        return find(current->left, key);
    }
    else if(comp == 2)
    {
        return find(current->right, key);
    }

    return NULL;
}

int swap_and_fix(dictionary_entry_t* left, dictionary_entry_t* right)
{
    if(left == NULL || right == NULL)
    {
        return -1;
    }

    if((compare(left->key, right->key) == 1)) // If left is bigger than current. Swap and go down left subtree.
    {
        swap(&left, &right); //
        left = right;

        return 1;
    }

    return 0;
}

void fix_tree(dictionary_entry_t* current)
{
    // What about the parent's left or right ptrs? They are invalid.
    if(current == NULL)
    {
        return;
    }

    if(swap_and_fix(current, current->left) == 1)
    {
        fix_tree(current->left);
    }

    if(swap_and_fix(current, current->right) == 1)
    {
        fix_tree(current->right);
    }

    return;
}

// Todo: When the entry is bigger than left and larger than right.
int add_entry(dictionary_entry_t* current, dictionary_entry_t* entry)
{
    if(current == NULL || entry == NULL)
    {
        printf("Invalid input: add_entry()\n");
    }

    if(swap_and_fix(entry, current->left == 1))
    {
        current->left = entry;
        fix_tree(current->left);

        return 1;
    }
    else if(swap_and_fix(entry, current->right) == 1)
    {
        current->right = entry;
        fix_tree(current->right);

        return 1;
    }


}

int dictionary_add(dictionary_t* dictionary, const char* key, void* value)
{
    if(dictionary == NULL)
    {
        printf("Dictionary not initialized\n");
        return -1;
    }

    dictionary_entry_t* entry = entry_init(key, value);
    return add_entry(dictionary->root, entry);
}

int dictionary_remove(dictionary_t* dictionary, const char* key)
{
    return 1;
}

void* dictionary_get(dictionary_t* dictionary, const char* key)
{
    return NULL;
}
