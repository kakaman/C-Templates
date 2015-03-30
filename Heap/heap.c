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

heap_t* heap_init(int data_size,
                  int (*compare)(void const*, void const*),
                  bool (*find)(void const*, void const*),
                  void (*modify)(void*, void const*),
                  void (*print)(void const*))
{
    heap_t* heap = malloc(sizeof(heap_t));
    heap->size = 0;
    heap->data_size = data_size;
    heap->print = print;
    heap->compare = compare;
    heap->find = find;
    heap->modify = modify;
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

void const* heap_remove_at(heap_t* heap, int location)
{
    void const* location_ptr = heap->data + (location * heap->data_size);
    void const* last_ptr = heap->data + (heap->size * heap->data_size);
    int comp_val = heap->compare(last_ptr, location_ptr);

    swap(heap, location, heap->size);
    heap->size--;
    if (heap->size < (heap->allocated_size / 2))
    {
        heap->data = realloc(heap->data, (heap->allocated_size / 2) * heap->data_size);
        heap->allocated_size /= 2;
    }

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

// If you have to find.
void heap_modify_element(heap_t* heap, void const* data, void const* element, int* map)
{
    if(heap == NULL || data == NULL || element == NULL)
    {
        printf("\nInvalid inputs for heap_modify_element().\n");
        return;
    }

    int location = heap_find_location(heap, element);
    if(location == -1)
    {
        printf("Element not in heap.\n");
        return;
    }

    heap_modify_element_at(heap, data, location, map);
    return;
}

void heap_modify_element_at(heap_t* heap, void const* data, int location, int* map)
{
    if(heap == NULL || location == -1)
    {
        printf("\nInvalid inputs for heap_modify_element_at().\n\n");
        return;
    }

    // ToDo: Modify for decrease key. Copied from heap_remove_at().
    void const* location_ptr = heap->data + (location * heap->data_size);
    void const* parent_ptr = heap->data + (PARENT(location) * heap->data_size);

    heap->modify(location_ptr, data);
    int comp_val = heap->compare(parent_ptr, location_ptr);

    if(location == 0)
    {
        int location_index = heap->access(location_ptr);
        map[location_index] = 0;
        return;
    }


    if (comp_val == 1) // if parent > current heapify up.
    {
        heapify_up(heap, location, map);
    }

    return;
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
    if(heap->size < (heap->allocated_size / 2))
    {
        heap->data = realloc(heap->data, (heap->allocated_size / 2) * heap->data_size);
        heap->allocated_size /= 2;
    }

    heapify_down(heap, 0);

    return (heap->data + (heap->size * heap->data_size));
}

// Find an element in the Heap. O(n) because it has to search every element.
bool element_in_heap(heap_t* heap, void const* data)
{
    bool found = false;

    if(heap->size == 0 || data == NULL)
    {
        printf("Input values for element_in_heap() invalid.\n");
        return found;
    }

    void const* location_ptr = NULL;

    for(int location = 0; location < heap->size; location++)
    {
        location_ptr = heap->data + (location * heap->data_size);
        if(heap->find(location_ptr, data) == true)
        {
            found = true;
            break;
        }
    }

    return found;
}

int heap_find_location(heap_t* heap, void const* data)
{
    if(heap == NULL)
    {
        printf("Input values for heap_find_location() invalid.\n");
        return -1;
    }

    int location = -1;

    void const* location_ptr = NULL;

    for(int i = 0; i < heap->size; i++)
    {
        location_ptr = heap->data + (i * heap->data_size);
        if(heap->find(location_ptr, data) == true)
        {
            location = i;
            break;
        }
    }

    return location;
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


