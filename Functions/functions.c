/*
 * functions.c
 *
 *  Created on: Apr 3, 2015
 *      Author: vyshnav
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <limits.h>
#include <assert.h>

#include "functions.h"
#include "quicksort.h"

void print_bits_little_endian(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;

    for (int i = size - 1; i >= 0; i--)
    {
        for (int j = 7; j >= 0; j--)
        {
            byte = b[i] & (1 << j);
            byte >>= j;

            printf("%u", byte);
        }
    }

    printf("\n");

    return;
}

void print_bits_big_endian(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            byte = b[i] & (1 << j);
            byte >>= j;

            printf("%u", byte);
        }
    }

    printf("\n");

    return;
}

int strtob(char* str, int string_length, int bits_in_label)
{
    printf("strtob(%s):\n", str);

    int id = 0;
    for(int i = 0, count = 0; i < string_length && count < bits_in_label; i++ , count++)
    {
        (id << count) & (str[i] - '0');
    }


    printf("id: %d\n", id);

    print_bits_little_endian(sizeof(int), &id);

    printf("\n");
    return id;
}

int str_to_int(char* str, int string_length, int bits_in_label)
{
//    printf("str_to_int(%s):\n", str);
    int id = 0;
    for(int i = 0; i < string_length; i++)
    {
        if (str[i] == '1')
        {
            id *= 2;
            id += 1;
        }
        else if (str[i] == '0')
        {
            id *= 2;
            id += 0;
        }
    }

    //printf("id: %d\n\n",  id);
    return id;
}

int find_max(int* array, int size)
{
    int max = INT_MIN;
    for(int i = 0; i < size; i++)
    {
        if(max < array[i])
            max = array[i];
    }

    return max;
}

int find_min(int* array, int size)
{
    int min = INT_MAX;
    for(int i = 0; i < size; i++)
    {
        if(min > array[i])
            min = array[i];
    }

    return min;
}

void print_int(void* ptr)
{
    int value = *((int*) ptr);
    printf("%d", value);

    return;
}

int min_int(void* left_ptr, void* right_ptr)
{
    int left = *((int*) left_ptr);
    int right = *((int*) right_ptr);

    if(left < right)
    {
        return -1;
    }

    if(left > right)
    {
        return 1;
    }

    return 0;
}

int max_int(void* left_ptr, void* right_ptr)
{
    int left = *((int*) left_ptr);
    int right = *((int*) right_ptr);

    if(left < right)
    {
        return 1;
    }

    if(left > right)
    {
        return -1;
    }

    return 0;
}

void parse_and_write(char* input_file, char* output_file)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    int num_nodes = 0;
    int node_number = 0;
    int num_bits = 0;
    int* vertex_array = malloc(sizeof(int) * 200000);

    FILE* file = fopen(input_file, "r");

    read = getline(&line, &len, file);
    sscanf(line, "%d %d", &num_nodes, &num_bits);

    while ((read = getline(&line, &len, file)) != -1)
    {
        vertex_array[node_number] = str_to_int(line, (int)len, num_bits);
        node_number++;
    }
    printf("Invalid input args.\n");

    free(line);

    fclose(file);

    qs_t sort;
    qs_init(&sort, vertex_array, num_nodes, min_int, print_int);

    printf("Invalid input args.\n");
    quicksort(&sort, 0, num_nodes - 1);

    printf("Invalid input args.\n");
    FILE* output = fopen(output_file, "w");

    fprintf(output, "%d %d", num_nodes, num_bits);

    for(int i = 0; i < 200000; i++)
    {
        fprintf(output, "%d", vertex_array[i]);
    }

    fclose(output);

    return;
}

int main(int argc, char** argv)
{
//    str_to_int("0000", 4, 4);
//    strtob("0000", 4, 4);
//    str_to_int("0001", 4, 4);
//    strtob("0001", 4, 4);
//    str_to_int("0010", 4, 4);
//    strtob("0010", 4, 4);
//    str_to_int("0011", 4, 4);
//    strtob("0011", 4, 4);
//    str_to_int("0100", 4, 4);
//    strtob("0100", 4, 4);
//    str_to_int("0101", 4, 4);
//    strtob("0101", 4, 4);
//    str_to_int("0110", 4, 4);
//    strtob("0110", 4, 4);
//

    if(argc != 3)
    {
        printf("Invalid input args.\n");
        exit(-1);
    }

    parse_and_write(argv[1], argv[2]);

    return 1;
}
