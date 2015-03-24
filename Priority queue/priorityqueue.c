#include <stdio.h>
#include <stdlib.h>
#include "priorityqueue.h"

p_queue_t* p_queue_init(int size)
{
    p_queue_t* p_queue = malloc(sizeof(p_queue_t));

    p_queue->array = malloc(sizeof(p_queue_data_t) * size);
    p_queue->allocated = size;
    p_queue->num_elements = 1;

    return p_queue;
}

void p_queue_insert(p_queue_t* p_queue, void* data, int priority)
{
    p_queue_data_t* inserted;
    int size;
    int temp;

    if (p_queue->num_elements >= p_queue->allocated)
    {
        p_queue->allocated *= 2;
        inserted = p_queue->array = realloc(p_queue->array, sizeof(p_queue_data_t) * p_queue->allocated);
    }
    else
    {
        inserted = p_queue->array;
    }

    size = p_queue->num_elements++;

    // Append at end, then up heap
    while ((temp = size / 2) && priority < inserted[temp].priority)
    {
        inserted[size] = inserted[temp];
        size = temp;
    }

    inserted[size].data = data;
    inserted[size].priority = priority;

    return;
}

void heapify_down(p_queue_t* p_queue, p_queue_data_t* data, int size, int location)
{
    if(location >= p_queue->num_elements)
    {
        return;
    }

    if(data[location].priority > data[location + 1].priority)
    {
        location++;
    }

    if(data[p_queue->num_elements].priority < data[location].priority)
    {
        return;
    }

    data[size] = data[location];
    size = location;

    heapify_down(p_queue, data, size, location);

    return;
}

// Remove top item. Returns 0 if empty. *pri can be null.
void* p_queue_remove(p_queue_t* p_queue, int* priority)
{
    void* out;
    if (p_queue->num_elements == 1)
    {
        return 0;
    }

    p_queue_data_t* data = p_queue->array;

    out = data[1].data;
    if (priority)
    {
        *priority = data[1].priority;
    }

    // Place last item at top, then heapify down.
    p_queue->num_elements--;

    int size = 1;
    int temp;

    while ((temp = size * 2) < p_queue->num_elements)
    {
        if (temp + 1 < p_queue->num_elements && data[temp].priority > data[temp + 1].priority)
        {
            temp++;
        }

        if (data[p_queue->num_elements].priority <= data[temp].priority)
        {
            break;
        }

        data[size] = data[temp];
        size = temp;
    }

    data[size] = data[p_queue->num_elements];
    if (p_queue->num_elements < p_queue->allocated / 2 && p_queue->num_elements >= 16)
    {
        p_queue->array = realloc(p_queue->array, (p_queue->allocated /= 2) * sizeof(data[0]));
    }

    return out;
}

// Get the top element
void* p_queue_peek(p_queue_t* p_queue, int *pri)
{
    if (p_queue->num_elements == 1)
    {
        return 0;
    }

    if (pri)
    {
        *pri = p_queue->array[1].priority;
    }

    return p_queue->array[1].data;
}

int main()
{
    return 1;
}
