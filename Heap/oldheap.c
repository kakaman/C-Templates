#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "oldheap.h"

#define PARENT(x) (x == 2) ? 0 : (x / 2)
#define LEFT_CHILD(x) 2 * x + 1
#define RIGHT_CHILD(x) 2 * x + 2

// Todo: Expand if the node_t structure adds more elements.
heap_data_t* node_init(int data)
{
    heap_data_t* node = malloc(sizeof(heap_data_t));
    node->value = data;

    return node;
}

// Todo: Change if the node_t structure adds more elements.
void node_delete(heap_data_t* node)
{
    free(node);

    return;
}

// Internal swap function
void swap(heap_data_t** left, heap_data_t** right)
{
    heap_data_t* temp;
    temp = *left;
    *left = *right;
    *right = temp;
    temp = NULL;

}

// Initialize the heap. Returns an empty max heap with size 0.
heap_t* heap_init()
{
    heap_t* heap = malloc(sizeof(heap_t));
    heap->size = 0;
    heap->allocated_size = 0;
    heap->data = NULL;

    return heap;
}

// Clears the memory allocated for the heap.
void clear_heap(heap_t* heap)
{
    // Loops through the array and remove the data;
    for(int i = (heap->allocated_size - 1); i > -1; i--)
    {
        free(heap->data[i]);
    }

    free(heap->data);
    free(heap);

    return;
}

// Returns size of heap.
int get_heap_size(heap_t* heap)
{
    return heap->size;
}

int get_heap_top(heap_t* heap)
{
    if(heap->size == 0)
    {
        return -9999999;
    }
    else
    {
        return heap->data[0]->value;
    }

    return 0;
}

//// Builds a max heap given an array of numbers. Builds the heap in O(n) time.
//void create_max_heap(heap_t* heap, int* arr, int size)
//{
//    // Insertion into the heap without violating the shape property
//    for (int i = 0; i < size; i++)
//    {
//        if (heap->size == heap->allocated_size)
//        {
//            heap->elem = realloc(heap->elem, (heap->allocated_size * 2) * sizeof(heap_data_t*));
//            heap->allocated_size *= 2;
//        }
//        else if(heap->size == 0)
//        {
//            heap->elem = malloc(sizeof(heap_data_t*));
//            heap->allocated_size = 1;
//        }
//
//        heap->size++;
//
//        // Create new node and add to array.
//        heap_data_t* node = node_init(arr[i]);
//        heap->elem[(heap->size)] = node;
//    }
//
//    // Satisfy heap property
//    for (int i = ((heap->size - 1) / 2); i >= 0; i--)
//    {
//        heapify_max(heap, i);
//    }
//}

void heapify_down_max(heap_t* heap, int location)
{
    int max = 0;
    int left = LEFT_CHILD(location);
    int right = RIGHT_CHILD(location);

    if((left < heap->size) && (heap->data[left]->value > heap->data[location]->value))
    {
        max = left;
    }
    else
    {
        max = location;
    }

    if ((right < heap->size) && (heap->data[right]->value > heap->data[max]->value))
    {
        max = right;
    }
    print_heap(heap);
    if (max != location)
    {
        swap(&(heap->data[location]), &(heap->data[max]));
        heapify_down_max(heap, max);
    }

    return;
}

// Inserts node into the max heap.
void insert_max_heap(heap_t* heap, int data)
{
    heap_data_t* node = node_init(data);

    if (heap->size == 0)
    {
        heap->data = malloc(sizeof(heap_data_t*));
        heap->data[0] = node;
        heap->size++;
        heap->allocated_size++;

        return;
    }

    if (heap->size == heap->allocated_size)
    {
        heap->data = realloc(heap->data, (heap->allocated_size * 2) * sizeof(heap_data_t*));
        heap->allocated_size *= 2;
        memset(heap->data + (sizeof(heap_data_t*) * heap->size), 0, (sizeof(heap_data_t*) *(heap->allocated_size - heap->size)));
    }

    // Find the correct insertion point.
    int i = heap->size;
    while (i && node->value > heap->data[PARENT(i)]->value)
    {
        heap->data[i] = heap->data[PARENT(i)];
        i = PARENT(i);
    }

    heap->size++;
    heap->data[i] = node;

    return;
}

// Deletes node from top of max heap.
int heap_remove_top(heap_t* heap)
{
    int value = -1;
    heap_data_t* data = NULL;

    if (heap->size)
    {
        printf("Deleting node: %d\n", heap->data[0]->value);

        data = heap->data[0];
        heap->size--;
        heap->data[0] = heap->data[heap->size];

        if(heap->size == (heap->allocated_size / 2))
        {
            heap->data = realloc(heap->data, heap->size * sizeof(heap_data_t*));
            heap->allocated_size = heap->size;
        }

        heapify_down_max(heap, 0);
    }
    else
    {
        printf("Empty Maximum Heap\n");

        return value;
    }

    value = data->value;
    free(data);

    return value;
}

// Gets the max node from max heap
heap_data_t* get_min_node_max_heap(heap_t* heap, int location)
{
    heap_data_t* data = NULL;

    if (LEFT_CHILD(location) >= heap->size) // Todo: Make sure the condition works. >= ?
    {
        data = heap->data[location];

        return data;
    }

    heap_data_t* left = get_min_node_max_heap(heap, LEFT_CHILD(location));
    heap_data_t* right = get_min_node_max_heap(heap, RIGHT_CHILD(location));

    if (left <= right)
    {
        return left;
    }
    else
    {
        return right;
    }
}

//// Builds a min heap given an array of numbers. Builds the heap in O(n) time.
//void create_min_heap(heap_t* heap, int* arr, int size)
//{
//    for (int i = 0; i < size; i++)
//    {
//        if (heap->size == heap->allocated_size)
//        {
//            heap->elem = realloc(heap->elem, (heap->allocated_size * 2) * sizeof(heap_data_t*));
//            heap->allocated_size *= 2;
//        }
//        else if(heap->size == 0)
//        {
//            heap->elem = malloc(sizeof(heap_data_t*));
//            heap->allocated_size = 1;
//        }
//
//        heap->size++;
//
//        // Create new node and add to array.
//        heap_data_t* node = node_init(arr[i]);
//        heap->elem[(heap->size)] = node;
//    }
//
//    // Satisfy heap property
//    for (int i = ((heap->size - 1) / 2); i >= 0; i--)
//    {
//        heapify_min(heap, i);
//    }
//}

void heapify_down_min(heap_t* heap, int location)
{
    int min = 0;
    int left = LEFT_CHILD(location);
    int right = RIGHT_CHILD(location);

    if((left < heap->size) && (heap->data[left]->value < heap->data[location]->value))
    {
        min = left;
    }
    else
    {
        min = location;
    }

    if ((right < heap->size) && (heap->data[right]->value < heap->data[min]->value))
    {
        min = right;
    }

    if (min != location)
    {
        swap(&(heap->data[location]), &(heap->data[min]));
        heapify_down_min(heap, min);
    }

    return;
}

// Inserts node into the min heap.
void insert_min_heap(heap_t* heap, int data)
{
    heap_data_t* node = node_init(data);

    if (heap->size == 0)
    {
        heap->data = malloc(sizeof(heap_data_t*));
        heap->data[0] = node;
        heap->size++;
        heap->allocated_size++;
        return;
    }

    if (heap->size == heap->allocated_size)
    {
        heap->data = realloc(heap->data, (heap->allocated_size * 2) * sizeof(heap_data_t*));
        heap->allocated_size *= 2;
        memset(heap->data + (sizeof(heap_data_t*) * heap->size), 0, (sizeof(heap_data_t*) *(heap->allocated_size - heap->size)));
    }

    int i = heap->size;
    while (i && node->value < heap->data[PARENT(i)]->value) // Find the correct insertion point.
    {
        heap->data[i] = heap->data[PARENT(i)];
        i = PARENT(i);
    }

    heap->size++;
    heap->data[i] = node;

    return;
}

// Deletes node from top of min heap.
int remove_min(heap_t* heap)
{
    heap_data_t* data = NULL;
    int value = -1;
    if (heap->size)
    {
        printf("Deleting node: %d\n", heap->data[0]->value);

        data = heap->data[0];
        heap->size--;
        heap->data[0] = heap->data[heap->size];

        if(heap->size == (heap->allocated_size / 2))
        {
            heap->data = realloc(heap->data, heap->size * sizeof(heap_data_t*));
            heap->allocated_size = heap->size;
        }

        heapify_down_min(heap, 0);
    }
    else
    {
        printf("Empty Minimum Heap\n");

        return value;
    }

    value = data->value;
    free(data);

    return value;
}

// Gets the max node from Min heap
heap_data_t* get_min_node(heap_t* heap, int i)
{
    heap_data_t* data = NULL;

    int l_child = LEFT_CHILD(i);
    int r_child = RIGHT_CHILD(i);

    if (l_child >= heap->size)
    {
        data = heap->data[i];

        return data;
    }

    heap_data_t* left = get_min_node(heap, l_child);
    heap_data_t* right = get_min_node(heap, r_child);

    if (left >= right)
    {
        return left;
    }
    else
    {
        return right;
    }
}

// Prints the max heap by doing a level-order traversal
void print_heap(heap_t* heap)
{
    for (int i = 0; i < heap->size; i++)
    {
        printf("[%d]: %d ", i, heap->data[i]->value);
    }
    printf("\n");
}
