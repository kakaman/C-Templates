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

qs_t* quicksort_init(void* array,
                     int size,
                     int offset_primary,
                     int offset_secondary,
                     int (*compare)(void*, void*),
                     void (*print)(void*))
{
    qs_t* qs_array = malloc(sizeof(qs_t));
    qs_array->size = size;
    qs_array->allocated_size = size;
    qs_array->offset_primary = offset_primary;
    qs_array->offset_secondary = offset_secondary;
    qs_array->print = print;
    qs_array->compare = compare;
    qs_array->data = array;

    return qs_array;
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

int add_elements(qs_t* array)
{
    if(array == NULL)
        return -1;

    void* element_ptr = NULL;
    int total = 0;
    int offset = array->offset_primary;

    for(int i = 0; i < array->size; i++)
    {
         element_ptr = array->data[i];
         total += *(int*)(element_ptr + offset);
    }
    return total;
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

    int offset = array->offset_primary;
    int min_val = *(int*)(min_ptr + offset);
    int mid_val = *(int*)(mid_ptr + offset);
    int max_val = *(int*)(max_ptr + offset);

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
    int offset = array->offset_primary;

    void* min_ptr = array->data[min];
    int min_val = *(int*)(min_ptr + offset);

    int pivot_value = min_val;

    int pivot_index = min + 1;
    for (int j = min + 1; j <= max; j++)
    {
        void* temp = array->data[j];
        int val = *(int*)(temp + offset);

        if (val < pivot_value)
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
//    printf("Running Quicksort. Min: %d Max: %d.\n", min, max);
    if (max < min || min > max || min == max)
    {
//        printf("Invalid Min: %d Max: %d.\n\n", min, max);
        return 0;
    }

    int comparisons = (max - min); // The number of comparisons is the size of the array.

    int median = compute_median(array, min, max);
//    printf("Median: %d.\n", median);

    void* temp = array->data[min];
    array->data[min] = array->data[median];
    array->data[median] = temp;

    int pivot = partition(array, min, max);

//    printf("Pivot: %d.\n\n", pivot);

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

    void* temp = array->data[min];
    array->data[min] = array->data[median];
    array->data[median] = temp;

    int pivot = partition_secondary(array, min, max);

    comparisons += quicksort_secondary(array, min, pivot - 1);
    comparisons += quicksort_secondary(array, pivot + 1, max);

    return comparisons;
}

int compute_median_secondary(qs_t* array, int min, int max)
{
    int mid = min + (max - min) / 2;
    int offset = array->offset_secondary;

    void* min_ptr = array->data[min];
    void* mid_ptr = array->data[mid];
    void* max_ptr = array->data[max];

    int min_val = *(int*)(min_ptr + offset);
    int mid_val = *(int*)(mid_ptr + offset);
    int max_val = *(int*)(max_ptr + offset);

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
    int offset = array->offset_secondary;
    void* min_ptr = array->data[min];
    int min_val = *(int*)(min_ptr + offset);

    int pivot_value = min_val;

    int pivot_index = min + 1;
    for (int j = min + 1; j <= max; j++)
    {
        void* temp = array->data[j];
        int val = *(int*)(temp + offset);

        if (val < pivot_value)
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
