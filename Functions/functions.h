/*
 * functions.h
 *
 *  Created on: Apr 3, 2015
 *      Author: vyshnav
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

void print_bits_little_endian(size_t const size, void const * const ptr);
void print_bits_big_endian(size_t const size, void const * const ptr);

// Convert string to bit int.
int strtob(char* str, int string_length, int bits_in_label);
int str_to_bit(char* str, int string_length, int bits_in_label);

// Find Max/Min element of integer array;
int find_max(int* array, int size);
int find_min(int* array, int size);

#endif /* FUNCTIONS_H_ */
