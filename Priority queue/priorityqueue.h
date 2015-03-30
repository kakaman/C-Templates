/*
 * priorityqueue.h
 *
 *  Created on: Mar 24, 2015
 *      Author: vyshnav
 */

#pragma once

// Priority Queue implementation with a Minimum Heap as a base.
typedef struct _pqueue_t
{
    int size;
    int allocated_size;
    int data_size;
    int (*compare)(void const*, void const*);
    bool (*find)(void const*, void const*);
    void (*print)(void const*);
    void* data;
} pqueue_t;

// Constructor and destructor for pqueue.
pqueue_t* pqueue_init(int data_size, int (*compare)(void const*, void const*), bool (*find)(void const*, void const*), void (*print)(void const*));
void clear_pqueue(pqueue_t* pqueue);

void print_pqueue(pqueue_t* pqueue);
int get_pqueue_size(pqueue_t* pqueue); // Returns size of pqueue.

void pqueue_insert(pqueue_t* pqueue, void const* data);
void const* pqueue_remove(pqueue_t* pqueue, int location);

void const* get_pqueue_top(pqueue_t* pqueue);
void const* pqueue_remove_top(pqueue_t* pqueue);

void const* pqueue_find(pqueue_t* pqueue, void const* data);

// Decrease key functions.
void pqueue_decrease_key(pqueue_t* pqueue, void const* data);
void pqueue_decrease_key_at(pqueue_t* pqueue, int location);
