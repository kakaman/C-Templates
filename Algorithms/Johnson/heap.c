#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "heap.h"

#define PARENT(x) ((x - 1) / 2)
#define LEFT_CHILD(x) (2 * x + 1)
#define RIGHT_CHILD(x) (2 * x + 2)
#define INVALID_LOCATION -1

// Initialize the heap. Returns an empty max heap with size 0.
heap_t* heap_init(int location_offset,
                  int (*compare)(void const*, void const*),
                  bool (*find)(void const*, void const*),
                  void (*print)(void const*))
{
    heap_t* heap = malloc(sizeof(heap_t));
    heap->size = 0;
    heap->location_offset = location_offset;
    heap->print = print;
    heap->compare = compare;
    heap->find = find;
    heap->allocated_size = 1;
    heap->data = malloc(sizeof(void *));

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

    void* location_ptr = heap->data[location];
    void* min_ptr = location_ptr;

    if ((left < heap->size) && (heap->compare(heap->data[left], min_ptr) < 0))
    {
        void* left_ptr = heap->data[left];
        min = left;
        min_ptr = left_ptr;
    }

    if ((right < heap->size) && (heap->compare(heap->data[right], min_ptr) < 0))
    {
        void* right_ptr = heap->data[right];
        min = right;
        min_ptr = right_ptr;
    }

    if (min != location)
    {
        int offset = heap->location_offset;

        int* min_location = (int *)(min_ptr + offset);
        *min_location = location;

        int* location_location = (int *)(location_ptr + offset);
        *location_location = min;

        heap->data[location] = heap->data[min];
        heap->data[min] = location_ptr;

        heapify_down(heap, min);
    }

    return;
}


void heapify_up(heap_t* heap, int location)
{
    int parent = PARENT(location);
    void* parent_ptr = heap->data[parent];
    void* location_ptr = heap->data[location];

    if ((location > 0) && (heap->compare(parent_ptr, location_ptr) > 0))
    {
        int offset = heap->location_offset;

        int* parent_location = (int *)(parent_ptr + offset);
        *parent_location = location;

        int* location_location = (int *)(location_ptr + offset);
        *location_location = parent;

        heap->data[location] = heap->data[parent];
        heap->data[parent] = location_ptr;

        heapify_up(heap, parent);
    }

    return;
}

// Inserts node into the max heap.
void heap_insert(heap_t* heap, void* data)
{
    if (heap->size == heap->allocated_size)
    {
        heap->data = realloc(heap->data, (heap->allocated_size * 2) * sizeof(void*));
        heap->allocated_size *= 2;
    }

    heap->data[heap->size] = data;
    int *location_location = (int *)(data + heap->location_offset);
    *location_location = heap->size;

    heap->size++;

    heapify_up(heap, (heap->size - 1));

    return;
}


void* heap_remove_at(heap_t* heap, int location)
{
    void* location_ptr = heap->data[location];
    void* last_ptr = heap->data[heap->size - 1];
    int comp_val = heap->compare(last_ptr, location_ptr);

    int* location_location = (int *)(location_ptr + heap->location_offset);
    *location_location = INVALID_LOCATION;

    int* last_location = (int*)(last_ptr + heap->location_offset);
    *last_location = location;

    heap->data[location] = heap->data[heap->size - 1];
    heap->data[heap->size - 1] = location_ptr;

    heap->size--;

    if(heap->size < (heap->allocated_size / 2))
    {
        heap->data = realloc(heap->data, (heap->allocated_size / 2) * sizeof(void*));
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

    return location_ptr;
}

void heap_modify_element(heap_t* heap, void* location_ptr)
{
    int location = *(int *)(location_ptr + heap->location_offset);
    void* parent_ptr = heap->data[PARENT(location)];

    int comp_val = heap->compare(location_ptr, parent_ptr);
    if (comp_val < 0)
    {
        heapify_up(heap, location);
    }
    else
    {
        heapify_down(heap, location);
    }

    return;
}

void const* get_heap_top(heap_t* heap)
{
    if (heap->size == 0)
    {
        return NULL;
    }

    return heap->data[0];
}

// Deletes node from top of max heap.
void* heap_remove_top(heap_t* heap)
{
    if(heap->size == 0)
    {
        return NULL;
    }

    int offset = heap->location_offset;

    void* start_ptr = heap->data[0];
    void* last_ptr = heap->data[heap->size - 1];

    int* start_location = (int *)(start_ptr + offset);
    *start_location = INVALID_LOCATION;

    int* last_location = (int *)(last_ptr + offset);
    *last_location = 0;

    heap->data[0] = heap->data[heap->size - 1];
    heap->data[heap->size - 1] = start_ptr;
    heap->size--;

    if(heap->size < (heap->allocated_size / 2))
    {
        heap->data = realloc(heap->data, (heap->allocated_size / 2) * sizeof(void*));
        heap->allocated_size /= 2;
    }

    heapify_down(heap, 0);

    return (heap->data[heap->size]);
}

// Find an element in the Heap. O(n) because it has to search every element.
bool element_in_heap(heap_t* heap, void const* location_ptr)
{
    if(heap->size == 0 || location_ptr == NULL)
        return false;

    int location = *(int *)(location_ptr + heap->location_offset);

    return ((location >= 0) && (location < heap->size));
}

// If not using an associative mapping.
int heap_find_location(heap_t* heap, void const* data)
{
    int location = INVALID_LOCATION;
    for(int i = 0; i < heap->size; i++)
    {
        void const* location_ptr = heap->data[i];
        if(heap->find(location_ptr, data) == true)
        {
            location = i;
            break;
        }
    }

    return location;
}

void const* get_element_at(heap_t* heap, int location)
{
    void const* location_ptr = heap->data[location];

    return location_ptr;
}

// Prints the max heap by doing a level-order traversal
void print_heap(heap_t* heap)
{
    if (heap->size == 0)
    {
        printf("Empty Heap\n");

        return;
    }

    printf("Heap->size: %d\n", heap->size);
    for (int i = 0; i < heap->size; i++)
    {
        printf(" [%d]: ", i);
        heap->print(heap->data[i]);
    }
    printf("\n");

    return;
}

typedef struct _heap_data_t
{
    int key;
    int heap_location;
    char value[0];
} heap_data_t;

void print_heap_data(void const* ptr)
{
    heap_data_t* data = (heap_data_t*) ptr;
    printf("(%d, %s, %d)\n", data->key, data->value, data->heap_location);

    return;
}

int min_heap_data(void const* left_ptr, void const* right_ptr)
{
    heap_data_t* left = (heap_data_t*) left_ptr;
    heap_data_t* right = (heap_data_t*) right_ptr;

    if(left->key < right->key)
    {
        return -1;
    }

    if(left->key > right->key)
    {
        return 1;
    }

    return 0;
}

bool find_heap_data(void const* left_ptr, void const* right_ptr)
{
    heap_data_t* left = (heap_data_t*) left_ptr;
    heap_data_t* right = (heap_data_t*) right_ptr;

    return (left->key == right->key);
}

//int main()
//{
//    heap_data_t** data = malloc(sizeof(heap_data_t*) * 10);
//    for(int i = 0; i < 10; i++)
//    {
//        data[i] = malloc(sizeof(heap_data_t) + sizeof(char) *  6);
//        data[i]->key = i + 1;
//        data[i]->heap_location = -1;
//        strcpy(data[i]->value, "Hello");
//    }
//
//    for(int i = 0; i < 10; i++)
//    {
//        printf("data[%d]: Key: %d Value %s\n", i, data[i]->key, data[i]->value);
//    }
//
//    int location_offset = offsetof(heap_data_t, heap_location);
//
//    heap_t* min_heap = heap_init(location_offset, min_heap_data, find_heap_data, print_heap_data);
//
//    for(int i = 0; i < 10; i++)
//    {
//        printf("Inserting into heap: %d.\n", i);
//        heap_insert(min_heap, data[i]);
//    }
//
//    //heap_data_t* temp = min_heap->data[0];
//    //printf("min_heap->[%d]: Key: %d Value %s\n", 0, temp->key, temp->value);
//
//    print_heap(min_heap);
//
//    strcpy(data[5]->value, "Thank");
//    data[5]->key = 0;
//    print_heap(min_heap);
//    heap_modify_element(min_heap, data[5]);
//    print_heap(min_heap);
//
//    for(int i = 0; i < 10; i++)
//    {
//        heap_data_t* top = heap_remove_top(min_heap);
//        print_heap(min_heap);
//        free(top);
//    }
//
//    free(data);
//    clear_heap(min_heap);
//
//    return 1;
//}
