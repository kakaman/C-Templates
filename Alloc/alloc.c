/** @file alloc.c */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

typedef struct __memory_t
{
    size_t previous_size; // This is the size of the previous block.
    size_t current_size; // Size of current memory
    union // All start/occupy the same memory location
    {
        struct
        {
            struct __memory_t* next; // Pointer to next segment.
            struct __memory_t* prev; // Pointer to the previous segment.
        };
        char mem[0];
    };
} memory_t;

size_t page_size = 0;
void* heap_start = NULL;
void* heap_end = NULL;
memory_t free_list;
memory_t* free_sentinel = &free_list;

const size_t ALIGNMENT = (0x1 << 3) - 1;
const size_t MIN_MEM_SIZE = sizeof(free_list.next) + sizeof(free_list.prev);
const size_t MEM_OVERHEAD = sizeof(free_list.previous_size) + sizeof(free_list.current_size);
const size_t FREE_BIT = 0x1;

inline size_t get_item_size(memory_t* item)
{
    return (item->current_size & ~FREE_BIT);
}

inline memory_t* get_item(void* ptr)
{
    return (memory_t*) (((char*) ptr) - __builtin_offsetof(memory_t, mem));
}

inline bool is_free_item(memory_t* item)
{
    return (item->current_size & FREE_BIT);
}

inline void heap_initialize()
{
    if (heap_start == NULL)
    {
        heap_start = sbrk(0);
        heap_end = heap_start;
        page_size = getpagesize();

        free_sentinel->previous_size = 0;
        free_sentinel->current_size = FREE_BIT;
        free_sentinel->prev = free_sentinel;
        free_sentinel->next = free_sentinel;
    }

    return;
}

inline memory_t* get_next_item(memory_t* item)
{
    memory_t* next_item = (memory_t*) (item->mem + get_item_size(item));
    if (next_item->mem >= ((char*) heap_end))
        return NULL;

    return next_item;
}

inline memory_t* get_prev_item(memory_t* item)
{
    if ((void*) item <= heap_start)
        return NULL;

    void* prev_item = ((char*) item - item->previous_size - MEM_OVERHEAD);

    return ((memory_t*) prev_item);
}

inline void set_item_size(memory_t* current_item, size_t size)
{
    current_item->current_size = size;
    memory_t* next_item = (memory_t*) (current_item->mem + size);
    next_item->previous_size = size;

    return;
}

void add_free_item(memory_t* free_item)
{

    free_item->current_size |= FREE_BIT;

    memory_t* prev_item = free_item;
    memory_t* next_item = free_item;
    memory_t* prev_free_item = free_sentinel;
    do
    {
        prev_free_item = prev_free_item->prev;
        if ((prev_free_item < free_item) ||
            (prev_free_item == free_sentinel))
        {
            break;
        }

        prev_item = get_prev_item(prev_item);
        if (prev_item == NULL)
        {
            prev_free_item = free_sentinel;
            break;
        }
        else if (is_free_item(prev_item) == true)
        {
            prev_free_item = prev_item;
            break;
        }

        next_item = get_next_item(next_item);
        if (next_item == NULL)
        {
            prev_free_item = free_sentinel->prev;
            break;
        }
        else if (is_free_item(next_item) == true)
        {
            prev_free_item = next_item->prev;
            break;
        }
    } while (true);

    free_item->next = prev_free_item->next;
    free_item->next->prev = free_item;
    free_item->prev = prev_free_item;
    prev_free_item->next = free_item;

    return;
}

void remove_free_item(memory_t* free_item)
{
    free_item->next->prev = free_item->prev;
    free_item->prev->next = free_item->next;

    free_item->current_size &= ~FREE_BIT;

    return;
}

memory_t* get_free_item(size_t size)
{
    memory_t* freeItem = free_sentinel->next;

    while (freeItem != free_sentinel)
    {
        if (freeItem->current_size >= size)
        {
            return freeItem;
        }

        freeItem = freeItem->next;
    }

    return NULL;
}

memory_t* allocate_item(size_t size)
{
    size += MEM_OVERHEAD;
    size_t requested_size = ((size + page_size) / page_size) * page_size;

    void* ptr = sbrk(requested_size);
    if (ptr == (void*) -1)
        return NULL;

    heap_end = sbrk(0);
    size_t allocated_size = heap_end - ptr;

    size_t entry_size = allocated_size - MEM_OVERHEAD;
    memory_t* item = (memory_t*) (ptr - __builtin_offsetof(memory_t, current_size));
    set_item_size(item, entry_size);
    add_free_item(item);

    return item;
}

memory_t* split_item(memory_t* item, size_t size)
{
    memory_t* next_item = NULL;

    bool is_free = is_free_item(item);
    ssize_t remaining = (get_item_size(item) - size) - MEM_OVERHEAD;
    if (remaining > (ssize_t) MIN_MEM_SIZE)
    {
        set_item_size(item, size);
        if (is_free)
            item->current_size |= FREE_BIT;

        next_item = get_next_item(item);
        set_item_size(next_item, remaining);
        add_free_item(next_item);
    }

    return next_item;
}

memory_t* combine_items(memory_t* current_item, memory_t* next_item)
{
    bool is_free = is_free_item(current_item);
    size_t combined_size = get_item_size(current_item) + get_item_size(next_item) + MEM_OVERHEAD;
    remove_free_item(next_item);
    set_item_size(current_item, combined_size);
    if(is_free)
        current_item->current_size |= FREE_BIT;

    return current_item;
}

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size)
{
    /* Note: This function is complete. You do not need to modify it. */
    void *ptr = malloc(num * size);

    if (ptr)
        memset(ptr, 0x00, num * size);

    return ptr;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */

void *malloc(size_t size)
{
    heap_initialize();

    size = (size + ALIGNMENT) & ~ALIGNMENT;
    if (size < MIN_MEM_SIZE)
        size = MIN_MEM_SIZE;

    memory_t* free_item = get_free_item(size);
    if (free_item == NULL)
    {
        free_item = allocate_item(size);
        if (free_item == NULL)
            return NULL;
    }

    split_item(free_item, size);
    remove_free_item(free_item);

    return free_item->mem;
}

/**
 * Deallocate space in memory
 * 
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */

void free(void *ptr)
{
    // "If a null pointer is passed as argument, no action occurs."
    if (!ptr)
        return;

    memory_t* free_item = get_item(ptr);
    add_free_item(free_item);

    memory_t* next_item = get_next_item(free_item);
    if ((next_item != NULL) && is_free_item(next_item))
        free_item = combine_items(free_item, next_item);

    memory_t* prev_item = get_prev_item(free_item);
    if ((prev_item != NULL) && is_free_item(prev_item))
        combine_items(prev_item, free_item);

    return;
}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *    
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size)
{
    // "In case that ptr is NULL, the function behaves exactly as malloc()"
    if (!ptr)
        return malloc(size);

    // "In case that the size is 0, the memory previously allocated in ptr
    //  is deallocated as if a call to free() was made, and a NULL pointer
    //  is returned."
    if (!size)
    {
        free(ptr);
        return NULL;
    }

    size = (size + ALIGNMENT) & ~ALIGNMENT;
    if (size < MIN_MEM_SIZE)
        size = MIN_MEM_SIZE;

    memory_t* realloc_item = get_item(ptr);
    if(realloc_item->current_size == size)
        return ptr;

    memory_t* next_item = get_next_item(realloc_item);
    if((next_item != NULL) && is_free_item(next_item))
        realloc_item = combine_items(realloc_item, next_item);

    if (realloc_item->current_size > size)
    {
        split_item(realloc_item, size);

        return realloc_item->mem;
    }

    void* resized_memory = malloc(size);
    if (resized_memory == NULL)
        return NULL;

    memcpy(resized_memory, ptr, realloc_item->current_size);
    free(ptr);

    return resized_memory;
}
