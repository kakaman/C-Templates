/*
 * queue.h
 *
 *  Created on: Mar 11, 2015
 *      Author: vyshnav
 */

#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct _queue_node_t
{
    void* element; // Stored value
    struct queue_node_t* next; // Link to next node
    struct queue_node_t* prev; // Link to prev node
} queue_node_t;

typedef struct _queue_t
{
    queue_node_t* head; // Head of linked-list
    queue_node_t* tail; // Tail of linked-list
    int size; // Number of nodes in linked-list
} queue_t;

// Constructor and Destructor
void queue_init(queue_t* queue);
void queue_destroy(queue_t* queue);

// Queue Functions
void* queue_dequeue(queue_t* queue);
void* queue_at(queue_t* queue, int position);
void* queue_remove_at(queue_t* queue, int position);
void queue_enqueue(queue_t* queue, void* item);
unsigned int queue_size(queue_t* queue);

void queue_iterate(queue_t* queue, void (*iter_func)(void*, void*), void* arg);

#endif /* QUEUE_H_ */
