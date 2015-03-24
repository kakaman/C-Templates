#pragma once

typedef struct _heap_t
{
    int size;
    int allocated_size;
    int data_size;
    int (*compare)(void const*, void const*);
    void (*print)(void const*);
    void* data;
} heap_t;

// Constructor and destructor for heap.
heap_t* heap_init(int data_size, int (*compare)(void const*, void const*), void (*print)(void const*));
void clear_heap(heap_t* heap);

void print_heap(heap_t* heap);
int get_heap_size(heap_t* heap); // Returns size of heap.

void heap_insert(heap_t* heap, void const* data);
void const* heap_remove(heap_t* heap, int location);

void const* get_heap_top(heap_t* heap);
void const* heap_remove_top(heap_t* heap);
