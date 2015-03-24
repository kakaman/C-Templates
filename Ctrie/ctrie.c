/*
*  ctrie.c
*
*   Created on: Mar 11, 2015
*       Author: vyshnav
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "ctrie.h"

#define EMPTY_INODE 0
#define INODE 1
#define TOMBED_INODE 2
#define SNODE 3
#define CNODE 4
#define LNODE 5
#define SIZE 16

//lnode_t* lnode_init(const char* key, void* value, int data_size)
//{
//    lnode_t* lnode = malloc(sizeof(lnode_t));
//    lnode->snode = snode_init(key, value, data_size);
//    lnode->next = NULL;
//
//    return lnode;
//}
//
//void lnode_delete(lnode_t* lnode)
//{
//    lnode_t* temp = lnode;
//    while(lnode != NULL)
//    {
//        snode_delete(lnode->snode);
//        temp = lnode;
//        lnode = lnode->next;
//        free(temp);
//    }
//
//    return;
//}
//
//snode_t* lnode_remove(lnode_t* lnode, const char* key)
//{
//    snode_t* found = NULL;
//    lnode_t* temp = lnode;
//    while(lnode != NULL)
//    {
//        if(strcmp(lnode->snode->key, key))
//        {
//            temp->next = lnode->next;
//            found = lnode->snode;
//            free(lnode);
//
//            break;
//        }
//        temp = lnode;
//        lnode = lnode->next;
//    }
//
//    return found;
//}
//
//snode_t* lnode_lookup(lnode_t* lnode, const char* key)
//{
//    while(lnode != NULL)
//    {
//        if(strcmp(lnode->snode->key, key))
//        {
//            return lnode->snode;
//        }
//        lnode = lnode->next;
//    }
//
//    return NULL;
//}

void* realloc_clear(void* ptr, size_t element_size, int index, int* count_ptr)
{
    int current_count = *count_ptr;
    int new_count = ((index / current_count) + 1) * current_count;
    char* realloced_ptr = realloc(ptr, element_size * new_count);
    memset(realloced_ptr + (element_size * current_count), 0, element_size * (new_count - current_count));

    *count_ptr = new_count;

    return realloced_ptr;
}

void lnode_realloc(lnode_t* lnode)
{
    lnode->array = realloc_clear(lnode->array, sizeof(snode_t*), lnode->size, &lnode->allocated_size);
}

lnode_t* lnode_init()
{
        lnode_t* lnode = malloc(sizeof(lnode_t));
        lnode->array = malloc(sizeof(snode_t*) * 2);
        lnode->size = 0;
        lnode->allocated_size = 2;

        return lnode;
}

bool lnode_insert(lnode_t* lnode, const char* key, void* value, int data_size)
{
    if(lnode->size == (lnode->allocated_size - 1))
    {
        lnode_realloc(lnode);
    }
    lnode->array[lnode->size] = snode_init(key, value, data_size);

    return true;
}

// ToDo: Remove has to be modified.
snode_t* lnode_remove(lnode_t* lnode, const char* key)
{
    if(lnode == NULL)
        return NULL;

    snode_t* snode = NULL;
    int cpy = 0;
    snode_t** temp = malloc(sizeof(snode_t*) * lnode->allocated_size);

    for(int i = 0; i < lnode->size; i++)
    {
        if(strcmp(lnode->array[i]->key, key))
        {
            snode = lnode->array[i];
        }
        else
        {// ToDo: make sure this is correct.
            *temp[cpy] = *lnode->array[i];
            cpy++;
        }
    }
    free(lnode->array);
    lnode->array = temp;

    return snode;
}

lnode_t* lnode_cpy(lnode_t* left)
{
    if(left == NULL)
        return NULL;

    lnode_t* right = NULL;
    right = malloc(sizeof(lnode_t));
    right->size = left->size;
    right->allocated_size = left->allocated_size;
    right->array = malloc(sizeof(snode_t*) * left->allocated_size);

    memcpy(right->array, left->array, left->size * sizeof(snode_t*));

    return right;
}
snode_t* lnode_lookup(lnode_t* lnode, const char* key)
{
    if (lnode == NULL)
        return NULL;

    for (int i = 0; i < lnode->size; i++)
    {
        if (strcmp(lnode->array[i]->key, key))
        {
            return lnode->array[i];
        }
    }

    return NULL;
}

void lnode_delete(lnode_t* lnode)
{
    if (lnode == NULL)
        return;

    for (int i = 0; i < lnode->size; i++)
    {
        free(lnode->array[i]);
    }
    free(lnode->array);
    free(lnode);

    return;
}

//S-node init/delete functions.

unsigned int get_string_hash(char const* key)
{
    unsigned int hash = 0;
    while (*key)
        hash = hash * 101  +  *key++;

    return hash;
}

int get_slot(unsigned int hash, int depth)
{
    return (hash >> (4 * depth)) & 0xF;
}

snode_t* snode_init(const char* key, void* value, int data_size)
{
    if(key == NULL || value == NULL)
    {
        printf("snode_init() arguments invalid.\n");
        return NULL;
    }
    snode_t* snode = malloc(sizeof(snode_t));
    snode->data_size = data_size;
    snode->tomb = false;

    snode->key = malloc(strlen(key));
    strcpy(snode->key, key);

    snode->value = malloc(data_size);
    memcpy(snode->value, value, data_size);

    return snode;
}

void snode_create(snode_t* snode, const char* key, void* value, int data_size)
{
    snode = malloc(sizeof(snode_t));
    snode->data_size = data_size;
    snode->tomb = false;

    snode->key = malloc(strlen(key));
    strcpy(snode->key, key);

    snode->value = malloc(data_size);
    memcpy(snode->value, value, data_size);

    return;
}

void snode_delete(snode_t* snode)
{
    if(snode == NULL)
    {
        printf("snode_delete() arguments invalid.\n");
        return;
    }

    free(snode->key);
    free(snode->value);
    free(snode);

    snode->key = NULL;
    snode->value = NULL;
    snode = NULL;

    return;
}

snode_t* snode_cpy(snode_t* snode)
{
    snode_t* copy = snode_init(snode->key, snode->value, snode->data_size);

    return copy;
}
// C-node init/delete functions.

cnode_t* cnode_init(int level)
{
    cnode_t* cnode = malloc(sizeof(cnode)); // Allocate mem for cnode.

    cnode->level = level;
    cnode->num = 0;

    cnode->array = malloc(SIZE * sizeof(inode_t*)); // Allocate enough memory for 16 inodes.

    return cnode;
}

void cnode_delete(cnode_t* cnode)
{
    if(cnode == NULL)
    {
        printf("cnode_delete() argument invalid.\n");
        return;
    }

    for(int i = 0; i < SIZE; i++)
    {
        inode_delete(cnode->array[i]);
    }

    return;
}

// I-node init/delete functions.

inode_t* inode_init()
{
    inode_t* inode = malloc(sizeof(inode_t));
    inode->node_type = EMPTY_INODE;

    return inode;
}

void inode_delete(inode_t* inode)
{
    if(inode == NULL)
    {
        printf("inode_delete() argument invalid.\n");
        return;
    }

    // If inode is pointing to a cnode.
    if(inode->node_type == CNODE)
    {
        cnode_delete(inode->cnode);
    }
    else if(inode->node_type == SNODE) // If inode is pointing to a snode.
    {
        snode_delete(inode->snode);
    }

    free(inode);
    inode = NULL;

    return;
}

// Ctrie constructor/destructor functions.

ctrie_t* ctrie_init(int data_size) // Create the ctrie with the data_size given by user.
{
    ctrie_t* ctrie = malloc(sizeof(ctrie_t));
    ctrie->size = 0;
    ctrie->root = inode_init();

    if(data_size == 1) // If the input for value is of type char. Then this allows an input string of size 32.
    {
        ctrie->data_size = data_size * 32;
    }
    return ctrie;
}

void ctrie_destroy(ctrie_t* ctrie)
{
    inode_delete(ctrie->root);
    free(ctrie);

    return;
}

cnode_t* updated_insert_cnode(cnode_t* cnode, const char* key, void* value,  int data_size, int slot)
{
    cnode_t* updated = cnode_init(cnode->level);
    memcpy(updated->array, cnode->array, (16 * sizeof(inode_t)));
    updated->array[slot]->snode = snode_init(key, value, data_size);

    return updated;
}

// Recursive function.
inode_t* change_inode_type(snode_t* collision, const char* key, void* value, int data_size, unsigned int hash, int level)
{
    inode_t* updated_inode = inode_init();
    if(level == 8)
    {
        lnode_t* lnode = updated_inode->lnode;
        lnode = lnode_init();
        lnode->array[0] = snode_cpy(collision);
        lnode->array[2] = snode_init(key, value, data_size);
        updated_inode->node_type = LNODE;

        return updated_inode;
    }

    updated_inode->cnode = cnode_init(level);
    updated_inode->node_type = CNODE;
    int slot_prev = get_slot(get_string_hash(collision->key), level);
    int slot = get_slot(hash, level);

    if(slot == slot_prev)
    {
        updated_inode->cnode->array[slot] = inode_init();
        updated_inode->cnode->array[slot] = change_inode_type(collision, key, value, data_size, hash, level + 1);
    }
    else
    {
        updated_inode->cnode->array[slot_prev]->snode = snode_cpy(collision);
        updated_inode->cnode->array[slot]->snode = snode_init(key, value, data_size);
        updated_inode->cnode->num = 2;
    }

    return updated_inode;
}

// ToDo: Implement tombed inode case and complete hash collisions. Make sure the CAS is proper. Make sure leveling is proper.
bool internal_ctrie_insert(inode_t* inode, const char* key, void* value, int data_size, unsigned int hash, int level)
{
    cnode_t* updated = inode->cnode;
    bool CAS_return = false;
    int slot = get_slot(hash, level);

    if(level == 8)
    {
        lnode_t* updated_lnode = NULL;
        if(inode->node_type == LNODE)
        {
            updated_lnode = lnode_cpy(inode->lnode);
        }
        else
        {
            updated_lnode = lnode_init();
            updated_lnode->array[0] = snode_init(key, value, data_size);
            updated_lnode->size = 2;
        }

        CAS_return = __sync_bool_compare_and_swap(&inode->lnode, inode->lnode, updated_lnode);

        if(CAS_return == true)
        {
            inode->node_type = LNODE;
            return true;
        }
    }
    else if (inode->node_type == TOMBED_INODE)
    {
        // ToDo: implement contract functionality.
        return false;
    }
    else if(inode->node_type == EMPTY_INODE)
    {
        // ToDo: Make sure this is proper.
        updated = cnode_init(level);
        updated->array[slot] = inode_init();
        updated->array[slot]->node_type = SNODE;
        updated->array[slot]->snode = snode_init(key, value, data_size);

        CAS_return = __sync_bool_compare_and_swap(&inode->cnode, inode->cnode, updated);
        printf("Inode->cnode->level: %d\n", inode->cnode->level);

        if(CAS_return == true)
        {
            inode->node_type = CNODE;
            printf("Insert 1: Level: %d, Slot: %d\n", level, slot);
            return true;
        }
    }
    else if(inode->node_type == SNODE)
    {
        inode_t* updated_inode = change_inode_type(inode->snode, key, value, data_size, hash, level + 1);

        CAS_return = __sync_bool_compare_and_swap(&inode, inode, updated_inode);
        if(CAS_return == true)
        {
            inode->node_type = CNODE;
            return true;
        }
    }
    else if(inode->node_type == CNODE)
    {
        inode_t* slot_inode = inode->cnode->array[slot];
        if (slot_inode == NULL) // First time at this level.
        {
            inode_t* updated_node = inode_init();
            updated_node->node_type = SNODE;
            updated_node->snode = snode_init(key, value, data_size);

            CAS_return = __sync_bool_compare_and_swap(&inode->cnode->array[slot], inode->cnode->array[slot], updated_node);
            return CAS_return;
        }
    }

    return internal_ctrie_insert(inode->cnode->array[slot], key, value, data_size, hash, level + 1);
}
//    if (slot_inode->node_type == CNODE || slot_inode->node_type == SNODE)
//    {
//        return internal_ctrie_insert(inode->cnode->array[slot], key, value, data_size, hash, level + 1);
//    }
//    else if(slot_inode->node_type == EMPTY_INODE)
//    {
//        updated = updated_insert_cnode(inode->cnode, key, value, data_size, slot);
//
//        // ToDo: CAS on inode->cnode and updated.
//        CAS_return = __sync_bool_compare_and_swap(&inode->cnode, inode->cnode, updated);
//    }
//
//    if(CAS_return == true)
//    {
//        inode->node_type = INODE;
//    }


bool ctrie_insert(ctrie_t* ctrie, const char* key, void* value)
{
    if(ctrie == NULL || key == NULL || value == NULL)
    {
        printf("ctrie_insert() arguments invalid\n");
        return false;
    }

    unsigned int hash = get_string_hash(key);
    bool inserted = false;

    while (inserted == false)
    {
        inserted = internal_ctrie_insert(ctrie->root, key, value, ctrie->data_size, hash, 1);
    }

    ctrie->size++;

    return inserted;
}

snode_t* internal_ctrie_remove(inode_t* inode, const char* key, int data_size, unsigned int hash, int level)
{
    bool CAS_return = false;
    int slot = get_slot(hash, level);

    if(inode->node_type == LNODE)
    {
        lnode_t* updated_lnode = lnode_cpy(inode->lnode);
        snode_t* ret_val = lnode_remove(updated_lnode, key);

        CAS_return = __sync_bool_compare_and_swap(&inode->lnode, inode->lnode, updated_lnode);

        if(CAS_return == true)
        {
            return ret_val;
        }
        else
        {
            return NULL;
        }
    }
    if(inode->node_type == EMPTY_INODE)
    {
        printf("The (key, value) pair does not exist.\n");
        return NULL;
    }
    else if(inode->node_type == TOMBED_INODE)
    {
        printf("Tombed Inode. Run contract first.\n");
        return NULL;
    }

    inode_t* slot_inode = inode->cnode->array[slot];
    if(slot_inode->node_type == EMPTY_INODE)
    {
        printf("The (key, value) pair does not exist.\n");
        return NULL;
    }
    if(slot_inode->node_type == INODE)
    {
        return internal_ctrie_remove(slot_inode, key, data_size, hash, level + 1);
    }

    return NULL;
}


void* ctrie_remove(ctrie_t* ctrie, const char* key)
{
    if(ctrie == NULL || key == NULL)
    {
        printf("ctrie_insert() arguments invalid\n");
        return NULL;
    }

    snode_t* removed = NULL;
    unsigned int hash = get_string_hash(key);

    while(removed == NULL)
    {
        removed = internal_ctrie_remove(ctrie->root, key, ctrie->data_size, hash, 1);
    }

    return removed->value;
}

snode_t* internal_ctrie_lookup(inode_t* inode, const char* key, int data_size, unsigned int hash, int level)
{
    if(inode == NULL)
        return NULL;

    int slot = get_slot(hash, level);

    if(inode->node_type == LNODE)
    {
        return lnode_lookup(inode->lnode, key);
    }
    if(inode->node_type == EMPTY_INODE)
    {
        printf("The (key, value) pair does not exist.\n");
        return NULL;
    }
    else if(inode->node_type == TOMBED_INODE)
    {
        printf("Tombed Inode. Returned snode. But, should run contract first.\n");
        return inode->snode;
    }
    else if(inode->node_type == SNODE)
    {
        printf("(Key, Value) found.\n");
        return inode->snode;
    }
    else if (inode->node_type == CNODE)
    {
        inode_t* slot_inode = inode->cnode->array[slot];

        if (slot_inode == NULL || slot_inode->node_type == EMPTY_INODE)
        {
            printf("The (key, value) pair does not exist.\n");
            return NULL;
        }
        else if (slot_inode->node_type == SNODE)
        {
            return slot_inode->snode;
        }

    }
        return internal_ctrie_lookup(inode->cnode->array[slot], key, data_size, hash, level + 1);
}


snode_t* ctrie_lookup(ctrie_t* ctrie, const char* key)
{
    if(ctrie == NULL || key == NULL)
    {
        printf("ctrie_insert() arguments invalid\n");
        return NULL;
    }

    snode_t* lookup = NULL;
    unsigned int hash = get_string_hash(key);

    lookup = internal_ctrie_lookup(ctrie->root, key, ctrie->data_size, hash, 1);


    return lookup;
}

//int ctrie_lookup(const char* key);
//int ctrie_update(const char* key);

// Helpful Ctrie functions.

int get_num_elements(ctrie_t* ctrie)
{
    return ctrie->size;
}

int main()
{
//    printf("inode_init() test.\n");
//    inode_t* inode = inode_init();
//    inode->snode = snode_init("1", "The Number of bytes: 23", sizeof(char));
//    printf("inode->snode->key: %s\n", inode->snode->key);
//    printf("inode->snode->value: %s\n", (char*) inode->snode->value);

    printf("Calling ctrie_init(sizeof(char));\n");
    ctrie_t* ctrie = ctrie_init(sizeof(char));

    if(ctrie == NULL)
    {
        printf("Ctrie did not initialize.\n");
    }

    printf("Calling ctrie_insert(ctrie, Test String, 2);\n");
    bool ret_val = ctrie_insert(ctrie, "1", "Test String");
    printf("ctrie_insert() returned: %d\n", ret_val);
    ret_val = ctrie_insert(ctrie, "hello", "Test String 2");
    printf("ctrie_insert() returned: %d\n", ret_val);

    printf("Calling ctrie_lookup(ctrie, hello);\n");
    snode_t* snode = ctrie_lookup(ctrie, "hello");
    printf("ctrie_lookup() returned: %s\n", (char*)snode->value);

    printf(Calling)
    return 1;
}
