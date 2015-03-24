#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "heap.h"

#define PARENT(x) ((x - 1) / 2)
#define LEFT_CHILD(x) (2 * x + 1)
#define RIGHT_CHILD(x) (2 * x + 2)

// Todo: Expand if the node_t structure adds more elements.
void node_copy(heap_t* heap, int location, void const* data)
{
    memcpy(heap->data + (location * heap->data_size), data, heap->data_size);

    return;
}

// Internal swap function
void swap(heap_t *heap, int left, int right)
{
    char temp[heap->data_size];
    void* left_ptr = heap->data + (left * heap->data_size);
    void* right_ptr = heap->data + (right * heap->data_size);

    memcpy(temp, left_ptr, heap->data_size);
    memcpy(left_ptr, right_ptr, heap->data_size);
    memcpy(right_ptr, temp, heap->data_size);

    return;
}

// Initialize the heap. Returns an empty max heap with size 0.
heap_t* heap_init(int data_size, int (*compare)(void const*, void const*), void (*print)(void const*))
{
    heap_t* heap = malloc(sizeof(heap_t));
    heap->size = 0;
    heap->data_size = data_size;
    heap->print = print;
    heap->compare = compare;
    heap->allocated_size = 1;
    heap->data = malloc(data_size);

    return heap;
}

// Clears the memory allocated for the heap.
void clear_heap(heap_t* heap)
{
    free(heap->data);
    free(heap);

    return;
}

// Returns size of heap.
int get_heap_size(heap_t* heap)
{
    return heap->size;
}

void heapify_down(heap_t* heap, int location)
{
    int min = location;
    int left = LEFT_CHILD(location);
    int right = RIGHT_CHILD(location);

    void const* min_ptr = heap->data + (min * heap->data_size);
    void const* left_ptr = heap->data + (left * heap->data_size);
    if ((left < heap->size) && (heap->compare(left_ptr, min_ptr) < 0))
    {
        min = left;
        min_ptr = left_ptr;
    }

    void const* right_ptr = heap->data + (right * heap->data_size);
    if ((right < heap->size) && (heap->compare(right_ptr, min_ptr) < 0))
    {
        min = right;
    }

    if (min != location)
    {
        swap(heap, location, min);
        heapify_down(heap, min);
    }

    return;
}

void heapify_up(heap_t* heap, int location)
{
    int parent = PARENT(location);
    void const* parent_ptr = heap->data + (parent * heap->data_size);
    void const* child_ptr = heap->data + (location * heap->data_size);

    if(location > 0 && (heap->compare(parent_ptr, child_ptr) > 0))
    {
        swap(heap, parent, location);
        heapify_up(heap, parent);
    }

    return;
}

// Inserts node into the max heap.
void heap_insert(heap_t* heap, void const* data)
{
    if (heap->size == heap->allocated_size)
    {
        heap->data = realloc(heap->data, (heap->allocated_size * 2) * heap->data_size);
        heap->allocated_size *= 2;
    }

    // Find the correct insertion point.
    memcpy(heap->data + (heap->size * heap->data_size), data, heap->data_size);
    heap->size++;

    heapify_up(heap, (heap->size - 1));

    return;
}

void const* heap_remove(heap_t* heap, int location)
{
    void const* location_ptr = heap->data + (location * heap->data_size);
    void const* last_ptr = heap->data + (heap->size * heap->data_size);
    int comp_val = heap->compare(last_ptr, location_ptr);

    swap(heap, location, heap->size);
    heap->size--;

    if(comp_val < 0)
    {
        heapify_up(heap, location);
    }
    else if(comp_val > 0)
    {
        heapify_down(heap, location);
    }

    return (heap->data + (heap->size * heap->data_size));
}

void const* get_heap_top(heap_t* heap)
{
    if (heap->size == 0)
    {
        return NULL;
    }

    return heap->data;
}

// Deletes node from top of max heap.
void const* heap_remove_top(heap_t* heap)
{
    if(heap->size == 0)
    {
        return NULL;
    }

    swap(heap, 0, (heap->size - 1));
    heap->size--;
    heapify_down(heap, 0);

    return (heap->data + (heap->size * heap->data_size));
}

// Prints the max heap by doing a level-order traversal
void print_heap(heap_t* heap)
{
    if (heap->size == 0)
    {
        printf("Empty Heap\n");

        return;
    }

    for (int i = 0; i < heap->size; i++)
    {
        printf(" [%d]: ", i);
        heap->print(heap->data + (i * heap->data_size));

    }
    printf("\n");

    return;
}

void print_int(void const* ptr)
{
    int value = *((int*) ptr);
    printf("%d", value);

    return;
}

int min_int(void const* left_ptr, void const* right_ptr)
{
    int left = *((int*) left_ptr);
    int right = *((int*) right_ptr);

    if(left < right)
    {
        return -1;
    }

    if(left > right)
    {
        return 1;
    }

    return 0;
}

int max_int(void const* left_ptr, void const* right_ptr)
{
    int left = *((int*) left_ptr);
    int right = *((int*) right_ptr);

    if(left < right)
    {
        return 1;
    }

    if(left > right)
    {
        return -1;
    }

    return 0;
}

int main()
{
    heap_t* max = heap_init(sizeof(int), min_int, print_int);
//    heap_t* min = heap_init(sizeof(int), min_int, print_int);

    char * line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1)
    {
        int value;
        sscanf(line, "%d", &value);
        heap_insert(max, &value);

    }
    free(line);

    printf("Max:");
    print_heap(max);

    while(get_heap_size(max) > 0)
    {
        int value = *((int*) heap_remove_top(max));

        printf("%d ", value);
    }

    printf("\n");
//
//    for (int i = 0; i < 4; i++)
//    {
//        printf("%d\n", i);
//        if (array[i] < get_heap_top(max))
//        {
//            insert_max_heap(max, array[i]);
//            //printf("Max 1: ");
//            //print_heap(max);
//        }
//        else
//        {
//            insert_min_heap(min, array[i]);
//            //printf("Min 1: ");
//            //print_heap(min);
//        }
//
//        if ((get_heap_size(max) - get_heap_size(min)) > 1)
//        {
//            int data = remove_max(max);
//            insert_min_heap(min, data);
//            //printf("Min 2: ");
//            //print_heap(min);
//        }
//
//        if (get_heap_size(min) > get_heap_size(max))
//        {
//            int data = remove_min(min);
//            insert_max_heap(max, data);
//            //printf("Max 2: ");
//            //print_heap(max);
//        }
//
//    }
//    printf("Min: ");
//    print_heap(min);
//    printf("Max: ");
//    print_heap(max);

    return 1;
}
