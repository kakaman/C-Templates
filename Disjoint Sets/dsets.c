#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "dsets.h"

disjoint_set_t* create_disjoint_set(void* value)
{
    return NULL;
}

void delete_disjoint_set(disjoint_set_t* node)
{

}

void disjoint_set_union(disjoint_set_t* set_1, disjoint_set_t* set_2)
{

}

ds_node_t* disjoint_set_find(disjoint_set_t* set)
{
    return NULL;
}

ds_node_t* create_ds_node(void* value)
{
    ds_node_t* node = malloc(sizeof(ds_node_t));
    node->value = value;
    node->parent = NULL;
    node->rank = 0;
    return node;
}

void ds_node_union(ds_node_t* node1, ds_node_t* node2)
{
    if (node1->rank > node2->rank)
    {
        node2->parent = node1;
    }
    else if (node2->rank > node1->rank)
    {
        node1->parent = node2;
    }
    else
    { /* they are equal */
        node2->parent = node1;
        node1->rank++;
    }
}

ds_node_t* ds_node_find(ds_node_t* node)
{
    ds_node_t* temp;
    /* Find the root */
    ds_node_t* root = node;
    while (root->parent != NULL)
    {
        root = root->parent;
    }
    /* Update the parent pointers */
    while (node->parent != NULL)
    {
        temp = node->parent;
        node->parent = root;
        node = temp;
    }
    return root;
}

int main()
{
    return 1;
}
