/*
 * void_quicksort.c
 *
 *  Created on: Mar 18, 2015
 *      Author: vyshnav
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "quicksort.h"

// Helper functions.

// Copies data into an element in the array.
void element_copy(qs_t* array, int location, void const* data)
{
    memcpy(array->data + (location * array->data_size), data, array->data_size);

    return;
}

// Swap function.
void swap(qs_t* array, int left, int right)
{
    char temp[array->data_size];
    void* left_ptr = array->data + (left * array->data_size);
    void* right_ptr = array->data + (right * array->data_size);

    memcpy(temp, left_ptr, array->data_size);
    memcpy(left_ptr, right_ptr, array->data_size);
    memcpy(right_ptr, temp, array->data_size);

    return;
}

// Internal realloc_function.
void* realloc_clear(void* ptr, ssize_t element_size, int index, int* count_ptr)
{
    int current_count = *count_ptr;
    int new_count = ((index / current_count) + 1) * current_count;
    char* realloced_ptr = realloc(ptr, element_size * new_count);
    memset(realloced_ptr + (element_size * current_count), 0, element_size * (new_count - current_count));

    *count_ptr = new_count;

    return realloced_ptr;
}

// Initialize the quicksort array.
qs_t* quicksort_init(ssize_t data_size, int (*compare)(void*, void*), int (*access_primary)(void*), int (*access_secondary)(void*), void (*print)(void*))
{
    qs_t* array = malloc(sizeof(qs_t));
    array->size = 0;
    array->data_size = data_size;
    array->print = print;
    array->compare = compare;
    array->access_primary = access_primary;
    array->access_secondary = access_secondary;
    array->allocated_size = 1;
    array->data = malloc(data_size);

    return array;
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
    for(int i = 0; i < array->size; i++)
    {
        array->print(array->data + (i * array->data_size));
    }

    printf("Done Printing.\n");

    return;
}

int add_elements(qs_t* array)
{
    if(array == NULL)
        return -1;

    void* element_ptr = NULL;
    int total = 0;

    for(int i = 0; i < array->size; i++)
    {
         element_ptr = array->data + (i * array->data_size);
         total += array->access_primary(element_ptr);
    }
    return total;
}

// Insert element into the quicksort array.
void quicksort_insert(qs_t* array, void* data)
{
    if(array->size == array->allocated_size)
    {
        // Might give an error.
//        array->data = realloc_clear(array->data, array->data_size, array->allocated_size * 2, array->allocated_size);

        // Another option
        array->data = realloc(array->data, array->allocated_size * 2 * array->data_size);
        array->allocated_size *= 2;
    }

    memcpy(array->data + (array->size * array->data_size), data, array->data_size);
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
        array->data = realloc(array->data, (array->allocated_size * 2) * array->data_size);
    }

    // Return element pointed at array->size.
    return array->data + ((array->size - 1) * array->data_size);
}

// Remove element at index.
void* quicksort_remove_at(qs_t* array, int index)
{
    if(array == NULL || index < 0 || index >= array->size)
    {
        return NULL;
    }

    swap(array, index, array->size);
    array->size--;

    // Run quicksort after removing element.
    quicksort(array, 0, (array->size - 1));

    return  array->data + (array->size * array->data_size);
}

// Return data at index.
void* quicksort_data_at(qs_t* array, int index)
{
    if(array == NULL || index < 0 || index >= array->size)
    {
        return NULL;
    }

    void* location_ptr = array->data + (index * array->data_size);

    return location_ptr;
}

int compute_median(qs_t* array, int min, int max)
{
    int mid = min + (max - min) / 2;

    void* min_ptr = array->data + (min * array->data_size);
    void* mid_ptr = array->data + (mid * array->data_size);
    void* max_ptr = array->data + (max * array->data_size);

    int min_val = array->access_primary(min_ptr);
    int mid_val = array->access_primary(mid_ptr);
    int max_val = array->access_primary(max_ptr);

    if((min_val <= mid_val && min_val >= max_val) || (min_val >= mid_val && min_val <= max_val))
    {
         return min;
    }

    if((mid_val <= min_val && mid_val >= max_val) || (mid_val >= min_val && mid_val <= max_val))
    {
        return mid;
    }

    return max;
}

int partition(qs_t* array, int min, int max)
{
    void* min_ptr = array->data + (min * array->data_size);
    int min_val = array->access_primary(min_ptr);

    int pivot_value = min_val;

    int pivot_index = min + 1;
    for (int j = min + 1; j <= max; j++)
    {
        void* temp = array->data + (j * array->data_size);
        int val = array->access_primary(temp);

        if (val < pivot_value)
        {
            swap(array, j, pivot_index);
            pivot_index++;
        }
    }

    swap(array, (pivot_index - 1), min);

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

    swap(array, median, min);

    int pivot = partition(array, min, max);

    comparisons += quicksort(array, min, pivot - 1);
    comparisons += quicksort(array, pivot + 1, max);

    return comparisons;
}

int quicksort_secondary(qs_t* array, int min, int max)
{
    if (max < min || min > max || min == max)
    {
        return 0;
    }

    int comparisons = (max - min); // The number of comparisons is the size of the array.

    int median = compute_median_secondary(array, min, max);

    swap(array, median, min);

    int pivot = partition_secondary(array, min, max);

    comparisons += quicksort_secondary(array, min, pivot - 1);
    comparisons += quicksort_secondary(array, pivot + 1, max);

    return comparisons;
}

int compute_median_secondary(qs_t* array, int min, int max)
{
    int mid = min + (max - min) / 2;

    void* min_ptr = array->data + (min * array->data_size);
    void* mid_ptr = array->data + (mid * array->data_size);
    void* max_ptr = array->data + (max * array->data_size);

    int min_val = array->access_secondary(min_ptr);
    int mid_val = array->access_secondary(mid_ptr);
    int max_val = array->access_secondary(max_ptr);

    if((min_val <= mid_val && min_val >= max_val) || (min_val >= mid_val && min_val <= max_val))
    {
         return min;
    }

    if((mid_val <= min_val && mid_val >= max_val) || (mid_val >= min_val && mid_val <= max_val))
    {
        return mid;
    }

    return max;
}

int partition_secondary(qs_t* array, int min, int max)
{
    void* min_ptr = array->data + (min * array->data_size);
    int min_val = array->access_secondary(min_ptr);

    int pivot_value = min_val;

    int pivot_index = min + 1;
    for (int j = min + 1; j <= max; j++)
    {
        void* temp = array->data + (j * array->data_size);
        int val = array->access_secondary(temp);

        if (val < pivot_value)
        {
            swap(array, j, pivot_index);
            pivot_index++;
        }
    }

    swap(array, (pivot_index - 1), min);

    return (pivot_index - 1);
}
