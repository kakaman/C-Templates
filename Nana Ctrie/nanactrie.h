/** @file nanactrie.h */
#ifndef __DS_H__
#define __DS_H__
#include <pthread.h>

typedef struct _data_t
{
    char* key;
    char* value;
    unsigned long rev;
} data_t;

typedef struct _ctrie_t
{
    pthread_mutex_t mutex;
    int ref_count;
    void *volatile freed_nodes_list;
    void* root;
} ctrie_t;

ctrie_t* create_ctrie();
void destroy_ctrie(ctrie_t* ctrie);

data_t ctrie_get_data(ctrie_t* ctrie, char *key);
data_t* ctrie_delete_data(ctrie_t* ctrie, data_t* data);
data_t* ctrie_add_data(ctrie_t* ctrie, data_t* data);
data_t* ctrie_update_data(ctrie_t* ctrie, data_t* data);

void free_data(data_t* data);

void load_ctrie(ctrie_t* ctrie);
void save_ctrie(ctrie_t* ctrie);

#endif
