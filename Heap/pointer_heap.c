#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "pointer_heap.h"

#define PARENT(x) ((x - 1) / 2)
#define LEFT_CHILD(x) (2 * x + 1)
#define RIGHT_CHILD(x) (2 * x + 2)

typedef struct _heap_data_t
{
    int key;
    int heap_location;
    char* value;
}heap_data_t;

void node_copy(heap_t* heap, int location, void const* data)
{
    memcpy(heap->data + location, data, heap->data_size);

    return;
}

// Initialize the heap. Returns an empty max heap with size 0.
heap_t* heap_init(int data_size, int location_offset,
                  int (*compare)(void const*, void const*),
                  bool (*find)(void const*, void const*),
                  void (*print)(void const*))
{
    heap_t* heap = malloc(sizeof(heap_t));
    heap->size = 0;
    heap->data_size = data_size;
    heap->location_offset = location_offset;
    heap->print = print;
    heap->compare = compare;
    heap->find = find;
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

    void* location_ptr = heap->data[location];
    void* min_ptr = heap->data[min];
    void* left_ptr = heap->data[left];

    if ((left < heap->size) && (heap->compare(left_ptr, min_ptr) < 0))
    {
        min = left;
        min_ptr = left_ptr;
    }

    void const* right_ptr = heap->data[right];
    if ((right < heap->size) && (heap->compare(right_ptr, min_ptr) < 0))
    {
        min = right;
        min_ptr = right_ptr;
    }

    if (min != location)
    {
        int offset = heap->location_offset;

        int *min_location = (int *)(min_ptr + offset);
        *min_location = location;

        int *location_location = (int *)(location_ptr + offset);
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

        int *parent_location = (int *)(parent_ptr + offset);
        *parent_location = location;

        int *location_location = (int *)(location_ptr + offset);
        *location_location = parent;

        heap->data[location] = heap->data[parent];
        heap->data[parent] = location_ptr;

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

    heap->data[heap->size] = data;
    int *location_location = (int *)(heap->data[heap->size] + heap->location_offset);
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

    *(int*)(location_ptr + heap->location_offset) = -1;
    *(int*)(last_ptr + heap->location_offset) = location;

    heap->data[location] = heap->data[heap->size - 1];
    heap->data[heap->size - 1] = location_ptr;

    heap->size--;

    if(heap->size < (heap->allocated_size / 2))
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

    return location_ptr;
}

void heap_modify_element(heap_t* heap, void* location_ptr)
{
    int location = *(int *)(location_ptr + heap->location_offset);
    void* parent_ptr = heap->data[PARENT(location)];

    int comp_val = heap->compare(location_ptr, parent_ptr);
    printf("Comparison Val: %d Location: %d\n", comp_val, location);

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

    int* start_loc = (int *)(start_ptr + offset);
    *start_loc = -1;

    int* last_loc = (int *)(last_ptr + offset);
    *last_loc = 0;


    heap->data[0] = heap->data[heap->size - 1];
    heap->data[heap->size] = start_ptr;
    heap->size--;

    if(heap->size < (heap->allocated_size / 2))
    {
        heap->data = realloc(heap->data, (heap->allocated_size / 2) * heap->data_size);
        heap->allocated_size /= 2;
    }

    heapify_down(heap, 0);

    return (heap->data[heap->size]);
}

// Find an element in the Heap. O(n) because it has to search every element.
bool element_in_heap(heap_t* heap, void const* location_ptr)
{
    if(heap->size == 0 || location_ptr == NULL)
    {
        printf("Input values for element_in_heap() invalid.\n");
        return false;
    }

    int location = *(int *)(location_ptr + heap->location_offset);

    return (location >=0 && location < heap->size);
}

// If not using an associative mapping.
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
        location_ptr = heap->data[i];
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

    printf("\nHeap->size: %d\n", heap->size);
    for (int i = 0; i < heap->size; i++)
    {
        printf(" [%d]: ", i);
        heap->print(heap->data[i]);

    }
    printf("\n");

    return;
}

void print_heap_data(void const* ptr)
{
    heap_data_t* data = (heap_data_t*) ptr;
    printf("Key: %d Value: %s Heap_location: %d\n", data->key, data->value, data->heap_location);

    return;
}

int min_heap_data(void const* left_ptr, void const* right_ptr)
{
    heap_data_t* left = (heap_data_t*) left_ptr;
    heap_data_t* right = (heap_data_t*) right_ptr;

    printf("left->key: %d\n", left->key);
    printf("right->key: %d\n", right->key);
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
    return false;
}

int main()
{
    heap_data_t** data = malloc(sizeof(heap_data_t*) * 10);
    for(int i = 0; i < 10; i++)
    {
        data[i] = malloc(sizeof(heap_data_t));
        data[i]->key = i + 1;
        data[i]->heap_location = -1;
        data[i]->value = malloc(sizeof(char) * 6);
        data[i]->value = "Hello\n";
    }

    for(int i = 0; i < 10; i++)
    {
        printf("data[%d]: Key: %d Value %s\n", i, data[i]->key, data[i]->value);
    }

    int location_offset = offsetof(heap_data_t, heap_location);

    heap_t* min_heap = heap_init(sizeof(heap_data_t*), location_offset, min_heap_data, find_heap_data, print_heap_data);

    for(int i = 0; i < 10; i++)
    {
        printf("Inserting into heap: %d.\n", i);
        heap_insert(min_heap, data[i]);
    }

    //heap_data_t* temp = min_heap->data[0];
    //printf("min_heap->[%d]: Key: %d Value %s\n", 0, temp->key, temp->value);

    print_heap(min_heap);

    data[5]->value = "Thank\n";
    data[5]->key = 0;
    heap_modify_element(min_heap, data[5]);
    print_heap(min_heap);

    heap_remove_top(min_heap);
    print_heap(min_heap);

    return 1;
}
