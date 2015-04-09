/*
 * quicksort.c
 *
 *  Created on: Mar 18, 2015
 *      Author: Vyshnav Kakivaya
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "quicksort.h"

// Internal realloc_function.
void* qs_realloc_clear(void* ptr, ssize_t element_size, int index, int* count_ptr)
{
    int current_count = *count_ptr;
    int new_count = ((index / current_count) + 1) * current_count;
    char* realloced_ptr = realloc(ptr, element_size * new_count);
    memset(realloced_ptr + (element_size * current_count), 0, element_size * (new_count - current_count));

    *count_ptr = new_count;

    return realloced_ptr;
}

void qs_init(qs_t* qs, void* array, int size, int (*compare)(void*, void*), void (*print)(void*))
{
    qs->size = size;
    qs->allocated_size = size;
    qs->print = print;
    qs->compare = compare;
    qs->data = array;

    return;
}

qs_t* quicksort_init(void* array,
                     int size,
                     int (*compare)(void*, void*),
                     void (*print)(void*))
{
    qs_t* qs = malloc(sizeof(qs_t));
    qs_init(qs, array, size, compare, print);

    return qs;
}


void clear_quicksort(qs_t* array)
{
    free(array->data);
    free(array);

    return;
}

inline int get_size(qs_t* array)
{
    return array->size;
}

void print_array(qs_t* array)
{
    printf("\n");
    for(int i = 0; i < array->size; i++)
    {
        printf("[%d]: ", i);
        array->print(array->data[i]);
    }

    printf("\n");

    return;
}

// Insert element into the quicksort array.
void quicksort_insert(qs_t* array, void* data)
{
    if(array->size == array->allocated_size)
    {
        // Might give an error.
//        array->data = qs_realloc_clear(array->data, array->data_size, array->allocated_size * 2, array->allocated_size);

        // Another option
        array->data = realloc(array->data, array->allocated_size * 2 * sizeof(void*));
        array->allocated_size *= 2;
    }

    array->data[array->size] = data;
    array->size++;

    return;
}

// Only removes the element from the end.
void* quicksort_remove_end(qs_t* array)
{
    // If array DNE or no elements in array.
    if(array == NULL || array->size == 0)
    {
        return NULL;
    }

    // Decrement size and reallocate if space is available.
    array->size--;
    if (array->allocated_size == ((array->allocated_size / 2) - 1))
    {
        array->data = realloc(array->data, (array->allocated_size * 2) * sizeof(void*));
    }

    // Return element pointed at array->size.
    return array->data[array->size - 1];
}

// Remove element at index.
void* quicksort_remove_at(qs_t* array, int index)
{
    if(array == NULL || index < 0 || index >= array->size)
    {
        return NULL;
    }

    void* temp = array->data[index];
    array->data[index] = array->data[array->size - 1];
    array->data[array->size - 1] = temp;
    array->size--;

    // Run quicksort after removing element.
    quicksort(array, 0, (array->size - 1));

    return  array->data[array->size];
}

// Return data at index.
void* quicksort_data_at(qs_t* array, int index)
{
    if(array == NULL || index < 0 || index >= array->size)
    {
        return NULL;
    }

    void* location_ptr = array->data[index];

    return location_ptr;
}

int compute_median(qs_t* array, int min, int max)
{
    int mid = min + (max - min) / 2;

    void* min_ptr = array->data[min];
    void* mid_ptr = array->data[mid];
    void* max_ptr = array->data[max];

    int min_mid = array->compare(min_ptr, mid_ptr); // Returns 1 if min is larger than min. -1 if smaller.
    int mid_max = array->compare(mid_ptr, max_ptr); // Returns 1 if mid is larger than max. -1 if smaller.
    int min_max = array->compare(min_ptr, max_ptr); // Returns 1 if min is larger than max. -1 if smaller.

    // if((mid >= max && mid <= min) || (mid >= min && mid <= max)
    if((mid_max >=0 && min_mid >= 0) || (min_mid <= 0 && mid_max <= 0))
    {
        return mid;
    }
    // if((min <= mid && min >= max) || min >= mid && min <= max)
    else if((min_mid <= 0 && min_max >= 0) || (min_mid >= 0 && min_max <= 0))
    {
         return min;
    }

    return max;
}

int partition(qs_t* array, int min, int max)
{
    void* min_ptr = array->data[min];

    int pivot_index = min + 1;
    for (int j = min + 1; j <= max; j++)
    {
        void* temp = array->data[j];
        int comp = array->compare(min_ptr, temp);
        if (comp == 1)
        {
            void* temp = array->data[j];
            array->data[j] = array->data[pivot_index];
            array->data[pivot_index] = temp;
            pivot_index++;
        }
    }

    void* temp = array->data[min];
    array->data[min] = array->data[pivot_index - 1];
    array->data[pivot_index - 1] = temp;

    return (pivot_index - 1);
}

int quicksort(qs_t* array, int min, int max)
{
    if (max < min || min > max || min == max)
    {

        return 0;
    }

    int comparisons = (max - min); // The number of comparisons is the size of the array.

    int median = compute_median(array, min, max);

    void* temp = array->data[min];
    array->data[min] = array->data[median];
    array->data[median] = temp;

    int pivot = partition(array, min, max);

    comparisons += quicksort(array, min, pivot - 1);
    comparisons += quicksort(array, pivot + 1, max);

    return comparisons;
}
