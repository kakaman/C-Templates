/*
 * priorityqueue.c
 *
 *  Created on: Mar 24, 2015
 *      Author: vyshnav
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "priorityqueue.h"

#define PARENT(x) ((x - 1) / 2)
#define LEFT_CHILD(x) (2 * x + 1)
#define RIGHT_CHILD(x) (2 * x + 2)

// Todo: Expand if the node_t structure adds more elements.
void node_copy(pqueue_t* pqueue, int location, void const* data)
{
    memcpy(pqueue->data + (location * pqueue->data_size), data, pqueue->data_size);

    return;
}

// Internal swap function
void swap(pqueue_t *pqueue, int left, int right)
{
    char temp[pqueue->data_size];
    void* left_ptr = pqueue->data + (left * pqueue->data_size);
    void* right_ptr = pqueue->data + (right * pqueue->data_size);

    memcpy(temp, left_ptr, pqueue->data_size);
    memcpy(left_ptr, right_ptr, pqueue->data_size);
    memcpy(right_ptr, temp, pqueue->data_size);

    return;
}

// Initialize the pqueue. Returns an empty max pqueue with size 0.
pqueue_t* pqueue_init(int data_size, int (*compare)(void const*, void const*), bool (*find)(void const*, void const*), void (*print)(void const*))
{
    pqueue_t* pqueue = malloc(sizeof(pqueue_t));
    pqueue->size = 0;
    pqueue->data_size = data_size;
    pqueue->print = print;
    pqueue->compare = compare;
    pqueue->find = find;
    pqueue->allocated_size = 1;
    pqueue->data = malloc(data_size);

    return pqueue;
}

// Clears the memory allocated for the pqueue.
void clear_pqueue(pqueue_t* pqueue)
{
    free(pqueue->data);
    free(pqueue);

    return;
}

// Returns size of pqueue.
int get_pqueue_size(pqueue_t* pqueue)
{
    return pqueue->size;
}

void heapify_down(pqueue_t* pqueue, int location)
{
    int min = location;
    int left = LEFT_CHILD(location);
    int right = RIGHT_CHILD(location);

    void const* min_ptr = pqueue->data + (min * pqueue->data_size);
    void const* left_ptr = pqueue->data + (left * pqueue->data_size);
    if ((left < pqueue->size) && (pqueue->compare(left_ptr, min_ptr) < 0))
    {
        min = left;
        min_ptr = left_ptr;
    }

    void const* right_ptr = pqueue->data + (right * pqueue->data_size);
    if ((right < pqueue->size) && (pqueue->compare(right_ptr, min_ptr) < 0))
    {
        min = right;
    }

    if (min != location)
    {
        swap(pqueue, location, min);
        heapify_down(pqueue, min);
    }

    return;
}

void heapify_up(pqueue_t* pqueue, int location)
{
    int parent = PARENT(location);
    void const* parent_ptr = pqueue->data + (parent * pqueue->data_size);
    void const* child_ptr = pqueue->data + (location * pqueue->data_size);

    if(location > 0 && (pqueue->compare(parent_ptr, child_ptr) > 0))
    {
        swap(pqueue, parent, location);
        heapify_up(pqueue, parent);
    }

    return;
}

// Inserts node into the max pqueue.
void pqueue_insert(pqueue_t* pqueue, void const* data)
{
    if (pqueue->size == pqueue->allocated_size)
    {
        pqueue->data = realloc(pqueue->data, (pqueue->allocated_size * 2) * pqueue->data_size);
        pqueue->allocated_size *= 2;
    }

    // Find the correct insertion point.
    memcpy(pqueue->data + (pqueue->size * pqueue->data_size), data, pqueue->data_size);
    pqueue->size++;

    heapify_up(pqueue, (pqueue->size - 1));

    return;
}

void const* pqueue_remove(pqueue_t* pqueue, int location)
{
    void const* location_ptr = pqueue->data + (location * pqueue->data_size);
    void const* last_ptr = pqueue->data + (pqueue->size * pqueue->data_size);
    int comp_val = pqueue->compare(last_ptr, location_ptr);

    swap(pqueue, location, pqueue->size);
    pqueue->size--;

    if(comp_val < 0)
    {
        heapify_up(pqueue, location);
    }
    else if(comp_val > 0)
    {
        heapify_down(pqueue, location);
    }

    if(pqueue->allocated_size == ((pqueue->allocated_size/2) - 1))
    {
        pqueue->data = realloc(pqueue->data, (pqueue->allocated_size * 2) * pqueue->data_size);
    }

    return (pqueue->data + (pqueue->size * pqueue->data_size));
}

void const* get_pqueue_top(pqueue_t* pqueue)
{
    if (pqueue->size == 0)
    {
        return NULL;
    }

    return pqueue->data;
}

// Deletes node from top of max pqueue.
void const* pqueue_remove_top(pqueue_t* pqueue)
{
    if(pqueue->size == 0)
    {
        return NULL;
    }

    swap(pqueue, 0, (pqueue->size - 1));
    pqueue->size--;
    heapify_down(pqueue, 0);

    return (pqueue->data + (pqueue->size * pqueue->data_size));
}

// Find an element in the pqueue. O(n) because it has to search every element.
bool pqueue_find(pqueue_t* pqueue, void const* data)
{
    bool found = false;

    if(pqueue->size == 0 || data == NULL)
    {
        printf("Input values for heap_find invalid.\n");
        return found;
    }

    void const* location_ptr = NULL;

    for(int location = 0; location < pqueue->size; location++)
    {
        location_ptr = pqueue->data + (location * pqueue->data_size);
        if(pqueue->find(data, location_ptr) == true)
        {
            found = true;
            break;
        }
    }

    return found;
}

// Prints the max pqueue by doing a level-order traversal
void print_pqueue(pqueue_t* pqueue)
{
    if (pqueue->size == 0)
    {
        printf("Empty pqueue\n");

        return;
    }

    for (int i = 0; i < pqueue->size; i++)
    {
        printf(" [%d]: ", i);
        pqueue->print(pqueue->data + (i * pqueue->data_size));

    }
    printf("\n");

    return;
}


