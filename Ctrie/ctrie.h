/*
 * ctrie.h
 *
 *  Created on: Mar 11, 2015
 *      Author: vyshnav
 */

#ifndef CTRIE_H_
#define CTRIE_H_

#include <stdbool.h>

// Same as an snode
//typedef struct _ctrie_element_t
//{
//    const char* key;
//    void* value;
//} ctrie_element_t;

typedef struct _cnode_t
{
    // Num elements in the array.
    int level;
    int num;
    // A pointer to an array of pointers of type main_node_t
    struct _inode_t** array;
}cnode_t;

typedef struct _snode_t
{
    int data_size;
    char* key;
    void* value;
    bool tomb;
}snode_t;

// Used when there is complete hash collisions.
typedef struct _lnode_t
{
    int size;
    int allocated_size;
    snode_t** array;
}lnode_t;


typedef struct _inode_t
{
    int node_type;
    union
    {
        cnode_t* cnode;
        snode_t* snode;
        lnode_t* lnode;
    };
}inode_t;

typedef struct _ctrie_t
{
    int size;
    int data_size;
    inode_t* root;
}ctrie_t;

//snode functions
snode_t* snode_init(const char* key, void* value, int data_size);
void snode_delete(snode_t* snode);
void snode_create(snode_t* snode, const char* key, void* value, int data_size);
void snode_remove(snode_t* snode);
// inode functions
inode_t* inode_init();
void inode_delete(inode_t* inode);

// cnode functions
cnode_t* cnode_init();
void cnode_delete(cnode_t* cnode);

// Ctrie Constructor/Destructor
ctrie_t* ctrie_init();
void ctrie_destroy(ctrie_t* ctrie);

// Ctrie functions
bool ctrie_insert(ctrie_t* ctrie, const char* key, void* value);
snode_t* ctrie_remove(ctrie_t* ctrie, const char* key);
snode_t* ctrie_lookup(ctrie_t* ctrie, const char* key);
int ctrie_update(ctrie_t* ctrie, const char* key);

// I don't think this is needed.
int get_num_elements(ctrie_t* ctrie);

#endif /* CTRIE_H_ */
