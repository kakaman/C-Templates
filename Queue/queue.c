/*
*  queue.c
*
*   Created on: Mar 11, 2015
*       Author: vyshnav
*/

#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

queue_node_t* node_init(void* element)
{
    queue_node_t* node = malloc(sizeof(queue_node_t));
    node->element = element;
    node->next = NULL;
    node->prev = NULL;

    return node;
}
// Initializes queue structure.
void queue_init(queue_t* queue)
{
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;

    return;
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
queue_node_t* forwards(queue_t* queue, queue_node_t* current, int position)
{
    if(position == 0)
    {
        return current;
    }
    else
    {
        return forwards(queue, current->next, position - 1);
    }
}

// Helper function for finding node at position.
queue_node_t* reverse(queue_t* queue, queue_node_t* current, int position)
{
    if(position == 0)
    {
        return current;
    }
    else
    {
        return forwards(queue, current->prev, position - 1);
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

        return node;
    }
    else
    {
        // Why is this not working?
        node->prev->next = node->next;
        node->next->prev = node->prev;

        return node->element;
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
        node = forwards(queue, queue->head, position);
    }
    else
    {
        node = reverse(queue, queue->tail, (queue->size -1) - position); // Start from tail.
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
        node = forwards(queue, queue->head, position);
    }
    else
    {
        node = reverse(queue, queue->tail, (queue->size - 1) - position); // Start from tail.
    }

    return node->element;
}

// Stores element at the back of the queue.
void queue_enqueue(queue_t* queue, void* element)
{
    if(queue == NULL || element == NULL)
    {
        printf("Invalid arguments passed to queue_enqueue.\n");
        return;
    }

    // Allocate memory
    queue_node_t* end = node_init(element);

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
void queue_iterate(queue_t* queue, void (*iter_func)(void* , void* ), void* arg)
{
    queue_node_t* node;
    if (queue_size(queue) == 0)
    {
        return;
    }

    node = queue->head;
    while (node != NULL)
    {
        iter_func(node->element, arg);
        node = node->next;
    }
}
