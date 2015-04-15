/*
 * insert_quicksort.h
 *
 *  Created on: Mar 31, 2015
 *      Author: vyshnav
 */

#ifndef INSERT_QUICKSORT_H_
#define INSERT_QUICKSORT_H_

typedef struct _qs_t
{
    int size;
    int allocated_size;
    int data_size;
    int offset_primary;
    int offset_secondary;
    int (*compare)(void*, void*);
    void (*print)(void*);
    void* data;
}qs_t;

// Access Primary is the primary sort.
// Access Secondary is the secondary sort based on a different variable.
qs_t* empty_quicksort_init(ssize_t data_size,
                     int offset_primary,
                     int offset_secondary,
                     int (*compare)(void*, void*),
                     void (*print)(void*));

qs_t* quicksort_init(void* array,
                     ssize_t data_size,
                     int offset_primary,
                     int offset_secondary,
                     int (*compare)(void*, void*),
                     void (*print)(void*));

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

#endif /* INSERT_QUICKSORT_H_ */
