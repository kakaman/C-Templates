#pragma once

typedef struct _ds_node_t
{
    void* value;
    struct _ds_node_t* parent;
    int rank;
}ds_node_t;

typedef struct _disjoint_set_t
{
    ds_node_t* head;
    int size;
}disjoint_set_t;

disjoint_set_t* create_disjoint_set(void* value);
void delete_disjoint_set(ds_node_t* set);
void disjoint_set_union(disjoint_set_t* set_1, disjoint_set_t* set_2);
ds_node_t* disjoint_set_find(ds_node_t* node);

ds_node_t* create_ds_node(void* value);
void delete_ds_node(ds_node_t* set);
void ds_node_union(ds_node_t* node1, ds_node_t* node2);
ds_node_t* ds_node_find(ds_node_t* node);
