#pragma once

// This struct is meant to be inserted into the object using disjoint sets.
typedef struct _disjoint_set_t
{
    struct _disjoint_set_t* parent;
    int rank;
} disjoint_set_t;

disjoint_set_t* create_disjoint_set();
void delete_disjoint_set(disjoint_set_t* set);
void disjoint_set_union(disjoint_set_t* left, disjoint_set_t* right);
disjoint_set_t* disjoint_set_find(disjoint_set_t* node);
