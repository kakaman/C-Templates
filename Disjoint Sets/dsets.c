#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "dsets.h"

disjoint_set_t* create_disjoint_set()
{
    disjoint_set_t* set = malloc(sizeof(disjoint_set_t));
    set->parent = NULL;
    set->set_size = 0;

    return set;
}

void delete_disjoint_set(disjoint_set_t* set)
{
    set->parent = NULL;
    free(set);

    set = NULL;

    return;
}

// Attach to larger.
void disjoint_set_union(disjoint_set_t* left, disjoint_set_t* right)
{
    if (left->set_size > right->set_size)
    {
        right->parent = left;
        // left->set_size += right->set_size;
    }
    else if (right->set_size > left->set_size)
    {
        left->parent = right;
        // right->set_size += left->set_size;
    }
    else
    {
        right->parent = left;
        left->set_size++;
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

int main()
{
    return 1;
}
