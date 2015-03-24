/*
 * void_quicksort.h
 *
 *  Created on: Mar 18, 2015
 *      Author: vyshnav
 */

#ifndef QUICKSORT_H_
#define QUICKSORT_H_

typedef struct _qs_t
{
    int size;
    int allocated_size;
    int data_size;
    int (*compare)(void*, void*);
    int (*access_primary)(void*);
    int (*access_secondary)(void*);
    void (*print)(void*);
    void* data;
}qs_t;

// Constructor and destructor for Quick Sort.
qs_t* quicksort_init(ssize_t data_size, int (*compare)(void*, void*), int (*access_primary)(void*), int (*access_secondary)(void*), void (*print)(void*));
void clear_quicksort(qs_t* array);

void print_array(qs_t* array);
int get_size(qs_t* array);
int add_elements(qs_t* array);

void quicksort_insert(qs_t* array, void* data);
void* quicksort_remove_end(qs_t* array);
void* quicksort_remove_at(qs_t* array, int index);
void quicksort_remove_duplicates(qs_t* array);
void* quicksort_data_at(qs_t* array, int index);

int quicksort(qs_t* array, int min, int max);
int partition(qs_t* array, int min, int max);
int compute_median(qs_t* array, int min, int max);

// Secondary Quick Sort to sort subsets of the array based on a different integer variable.
int quicksort_secondary(qs_t* array, int min, int max);
int compute_median_secondary(qs_t* array, int min, int max);
int partition_secondary(qs_t* array, int min, int max);

void swap(qs_t* array, int index_one, int index_two);

#endif /* QUICKSORT_H_ */
