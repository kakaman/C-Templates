#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "dsets.h"

disjoint_set_t* create_disjoint_set()
{
    disjoint_set_t* set = malloc(sizeof(disjoint_set_t));
    set->parent = NULL;
    set->rank = 0;

    return set;
}

void delete_disjoint_set(disjoint_set_t* set)
{
    set->parent = set;
    free(set);

    set = NULL;

    return;
}

// Attach to larger.
void disjoint_set_union(disjoint_set_t* left, disjoint_set_t* right)
{
    disjoint_set_t* left_root = disjoint_set_find(left);
    disjoint_set_t* right_root = disjoint_set_find(right);

    if(left_root == right_root)
        return;

    if (left_root->rank < right_root->rank)
    {
        left_root->parent = right_root;
        // left->set_size += right->set_size;
    }
    else if (left_root->rank > right_root->rank)
    {
        right_root->parent = left_root;
        // right->set_size += left->set_size;
    }
    else
    {
        right_root->parent = left_root;
        left_root->rank++;
    }

    return;
}

disjoint_set_t* disjoint_set_find(disjoint_set_t* set)
{
    if (set == NULL)
    {
        return NULL;
    }
    else if (set->parent != set)
    {
        set->parent = disjoint_set_find(set->parent);
    }

    return set->parent;
}

void* disjoint_set_top(disjoint_set_t* set)
{

}
