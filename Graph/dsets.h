#pragma once

typedef struct _set_type_t
{
    int set_type;
} set_type_t;

// This struct is meant to be inserted into the object using disjoint sets.
typedef struct _disjoint_set_t
{
    struct _disjoint_set_t* parent;
    int rank;
} disjoint_set_t;

disjoint_set_t* create_disjoint_set();
disjoint_set_t* create_disjoint_set_top(void* top);
void delete_disjoint_set(disjoint_set_t* set);
disjoint_set_t* disjoint_set_union(disjoint_set_t* left, disjoint_set_t* right);
disjoint_set_t* disjoint_set_find(disjoint_set_t* node);
void* disjoint_set_find_top(disjoint_set_t* node);
