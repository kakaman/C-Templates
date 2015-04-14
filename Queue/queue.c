/*
*  queue.c
*
*   Created on: Mar 11, 2015
*       Author: vyshnav
*/

#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

// Standard implementation of queue.

queue_node_t* node_init(void* element, int data_size)
{
    queue_node_t* node = malloc(sizeof(queue_node_t));
    node->element = malloc(sizeof(data_size));
    memcpy(node->element, element, data_size);

    node->next = NULL;
    node->prev = NULL;

    return node;
}

queue_t* queue_init(void (*enumerate)(void*, void*), void (*print)(void*))
{
	queue_t* queue = malloc(sieof(queue_t));

	queue->head = NULL;
	queue->tail = NULL;
	queue->size = 0;
	queue->enumerate = enumerate;

	return queue;
}

// Frees all associated memory.
void queue_destroy(queue_t* queue)
{
    while (queue_size(queue) > 0)
    {
        queue_dequeue(queue);
    }

    return;
}

// Removes and returns element from front of queue. Returns NULL if the queue is empty.
void* queue_dequeue(queue_t* queue)
{
    queue_node_t* head;
    void* element;

    if (queue_size(queue) == 0)
    {
        return NULL;
    }

    head = queue->head;
    queue->head = queue->head->next;
    queue->size--;

    element = head->element;
    free(head);

    if (queue_size(queue) == 0) // WHen queue is empty.
    {
        queue->head = NULL;
        queue->tail = NULL;
    }

    return element;
}

// Helper function for finding node at position.
queue_node_t* next_element(queue_t* queue, queue_node_t* current, int position)
{
    if(position == 0)
    {
        return current;
    }
    else
    {
        return next_element(queue, current->next, position - 1);
    }
}

// Helper function for finding node at position.
queue_node_t* previous_element(queue_t* queue, queue_node_t* current, int position)
{
    if(position == 0)
    {
        return current;
    }
    else
    {
        return previous_element(queue, current->prev, position - 1);
    }
}

// Helper function for removing node.
void* remove_node(queue_t* queue, queue_node_t* node)
{
    if(queue  == NULL || node == NULL)
    {
        return NULL;
    }
    else if(node->prev == node) // Last/Only element in queue
    {
        queue->head = NULL;
        queue->tail = NULL;

        void* element = node->element;
        free(node);

        return element;
    }
    else
    {
        queue_node_t* prev = node->prev;
        queue_node_t* next = node->next;

        if(prev != NULL)
        	prev->next = next;

        if(next != NULL)
        	next->prev = prev;

        void* element = node->element;
        free(node);

        return element;
    }
}
// Removes and returns element at position. If the position is invalid, returns NULL.
void* queue_remove_at(queue_t* queue, int position)
{
    if (position < 0 || queue->size - 1 < position || queue == NULL)
    {
        return NULL;
    }
    // Decrement size
    queue->size--;

    queue_node_t* node = NULL;

    // Recursive calls
    if(position <= (queue->size / 2))
    {
        node = next_element(queue, queue->head, position);
    }
    else
    {
        node = previous_element(queue, queue->tail, (queue->size -1) - position); // Start from tail.
    }

    return remove_node(queue, node);
}

// Returns element located at position pos.
void* queue_at(queue_t* queue, int position)
{
    queue_node_t* node = NULL;
    if (position < 0 || queue->size - 1 < position || queue == NULL)
    {
        printf("Invalid arguments passed to queue_at.\n");
        return NULL;
    }
    else if (position <= (queue->size / 2))
    {
        node = next_element(queue, queue->head, position);
    }
    else
    {
        node = previous_element(queue, queue->tail, (queue->size - 1) - position); // Start from tail.
    }

    return node->element;
}

// Stores element at the back of the queue.
void queue_enqueue(queue_t* queue, void* element, int data_size)
{
    if(queue == NULL || element == NULL)
    {
        printf("Invalid arguments passed to queue_enqueue.\n");
        return;
    }

    // Allocate memory
    queue_node_t* end = node_init(element, data_size);

    if (queue_size(queue) == 0)
    {
        queue->head = end;
        queue->tail = end;
    }
    else
    {
        queue->tail->next = end;
        end->prev = queue->tail;
    }

    queue->tail = end;
    queue->size++;

    return;
}

// Returns number of items in the queue.
unsigned int queue_size(queue_t* queue)
{
    return queue->size;
}

// Helper function to apply operation on each element.
void queue_enumerate(queue_t* queue, void* arg)
{
    queue_node_t* node;
    if (queue_size(queue) == 0)
    {
        return;
    }

    node = queue->head;
    while (node != NULL)
    {
        queue->enumerate(node->element, arg);
        node = node->next;
    }
}

void queue_print(queue_t* queue)
{
    queue_node_t* node;
    if (queue_size(queue) == 0)
    {
        return;
    }

    node = queue->head;
    while (node != NULL)
    {
        queue->print(node->element);
        node = node->next;
    }
}
