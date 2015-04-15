/*
 * quicksort.h
 *
 *  Created on: Mar 18, 2015
 *      Author: Vyshnav Kakivaya
 */

#ifndef QUICKSORT_H_
#define QUICKSORT_H_

typedef struct _qs_t
{
    int size;
    int allocated_size;
    int (*compare)(void*, void*);
    void (*print)(void*);
    void** data;
} qs_t;

void qs_init(qs_t* qs, void* array, int size, int (*compare)(void*, void*), void (*print)(void*));

qs_t* quicksort_init(void* array, int size, int (*compare)(void*, void*), void (*print)(void*));

void clear_quicksort(qs_t* array);

void print_array(qs_t* array);
int get_size(qs_t* array);

void quicksort_insert(qs_t* array, void* data);
void* quicksort_remove_end(qs_t* array);
void* quicksort_remove_at(qs_t* array, int index);
void quicksort_remove_duplicates(qs_t* array);
void* quicksort_data_at(qs_t* array, int index);

int quicksort(qs_t* array, int min, int max);
int partition(qs_t* array, int min, int max);
int compute_median(qs_t* array, int min, int max);

#endif /* QUICKSORT_H_ */
