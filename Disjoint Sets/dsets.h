#pragma once

//typedef struct _ds_node_t
//{
//    void* value;
//    struct _ds_node_t* parent;
//    int rank;
//} ds_node_t;

// This struct is meant to be inserted into the object using disjoint sets.
typedef struct _disjoint_set_t
{
    disjoint_set_t* parent;
    int set_size;
} disjoint_set_t;

disjoint_set_t* create_disjoint_set();
void delete_disjoint_set(disjoint_set_t* set);
void disjoint_set_union(disjoint_set_t* left, disjoint_set_t* right);
disjoint_set_t* disjoint_set_find(disjoint_set_t* node);
