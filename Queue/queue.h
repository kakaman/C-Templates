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
    int data_size;
    struct queue_node_t* next; // Link to next node
    struct queue_node_t* prev; // Link to prev node
} queue_node_t;

typedef struct _queue_t
{
    queue_node_t* head; // Head of linked-list
    queue_node_t* tail; // Tail of linked-list
    int size; // Number of nodes in linked-list
    void (*enumerate)(void*, void*);
    void (*print)(void*);
} queue_t;

// Constructors and Destructors
queue_t* queue_init(void (*enumerate)(void*, void*), void (*print)(void*));
void queue_destroy(queue_t* queue);

// Queue Functions
void* queue_dequeue(queue_t* queue);
void* queue_at(queue_t* queue, int position);
void* queue_remove_at(queue_t* queue, int position);
void queue_enqueue(queue_t* queue, void* element, int data_size);
unsigned int queue_size(queue_t* queue);

void queue_enumerate(queue_t* queue, void* arg);
void queue_print(queue_t* queue);

#endif /* QUEUE_H_ */
