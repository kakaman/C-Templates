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
#include <math.h>
#include <stdint.h>

#include "functions.h"
#include "quicksort.h"

#define FMT_BUF_SIZE (CHAR_BIT*sizeof(uintmax_t)+1)

static const unsigned char table_random_bytes[256] =
{
       98,  6, 85,150, 36, 23,112,164,135,207,169,  5, 26, 64,165,219,
       61, 20, 68, 89,130, 63, 52,102, 24,229,132,245, 80,216,195,115,
       90,168,156,203,177,120,  2,190,188,  7,100,185,174,243,162, 10,
      237, 18,253,225,  8,208,172,244,255,126,101, 79,145,235,228,121,
      123,251, 67,250,161,  0,107, 97,241,111,181, 82,249, 33, 69, 55,
       59,153, 29,  9,213,167, 84, 93, 30, 46, 94, 75,151,114, 73,222,
      197, 96,210, 45, 16,227,248,202, 51,152,252,125, 81,206,215,186,
       39,158,178,187,131,136,  1, 49, 50, 17,141, 91, 47,129, 60, 99,
      154, 35, 86,171,105, 34, 38,200,147, 58, 77,118,173,246, 76,254,
      133,232,196,144,198,124, 53,  4,108, 74,223,234,134,230,157,139,
      189,205,199,128,176, 19,211,236,127,192,231, 70,233, 88,146, 44,
      183,201, 22, 83, 13,214,116,109,159, 32, 95,226,140,220, 57, 12,
      221, 31,209,182,143, 92,149,184,148, 62,113, 65, 37, 27,106,166,
        3, 14,204, 72, 21, 41, 56, 66, 28,193, 40,217, 25, 54,179,117,
      238, 87,240,155,180,170,242,212,191,163, 78,218,137,194,175,110,
};

static char binary_print_table[FMT_BUF_SIZE];

inline int get_slot(unsigned int hash, int depth)
{
    return (hash >> (4 * depth)) & 0xF;
}

char* binary_fmt(uintmax_t x, char buf[static FMT_BUF_SIZE], int num_bits)
{
    char *s = buf + FMT_BUF_SIZE;
    *--s = 0;
    if (!x)
    {
        *--s = '0';
    }

    for(int i = 0; (x > 0) || (i < num_bits); x/=2, i++)
    {
        *--s = '0' + x%2;
    }

    return s;
}

//void shifting_bytes()
//{
//    unsigned int bits = 0;
//    unsigned int long shift = 0xFF;
//    unsigned char char_shift = 0;
//    printf("Bits: %s\n", binary_fmt(shift, binary_print_table, 16));
//    printf("Shift: %s\n", binary_fmt(bits, binary_print_table, 16));
//
//    printf("Shifting left by 2 Bytes\n");
//    for(int i = 0; i < 3; i++)
//    {
//        bits = shift << (8 * i);
//        printf("%s\n", binary_fmt(bits, binary_print_table, 64));
//    }
//
//    printf("Replacing 2 Bytes\n");
//    bits = 0;
//    for(int i = 0; i < 3; i++)
//    {
//        bits = (bits | shift << (8 * i));
//        printf("%s\n", binary_fmt(bits, binary_print_table, 64));
//    }
//
//    // Decimal: 118
//    // Hex: 76
//    printf("\n");
//    unsigned char char_shift = 0xFF;
//    bits = 0;
//    for(int i = 0; i < 3; i++)
//    {
//        bits = (bits | char_shift << (8 * i));
//        printf("%s\n", binary_fmt(bits, binary_print_table, 64));
//    }
//
//    char str[5] = "Hello";
//
//    shift = 0;
//    printf("%s\n\n", binary_fmt(shift, binary_print_table, num_bits));
//    for(int i = 0; i < 5; i++)
//    {
//        shift = shift << (8 * i) | str[i];
//        printf("%s\n", binary_fmt(str[i], binary_print_table, num_char_bits));
//        printf("%s\n", binary_fmt(shift, binary_print_table, num_bits));
//    }
//}

void Pearson_64_char(const unsigned char *x, size_t len)
{
    size_t i, j;
    unsigned char hh[8];
    unsigned long int hash = 0;

    printf("Key: %s\n", x);
    for (j = 0; j < 8; j++)
    {
        unsigned char h = table_random_bytes[(x[0] + j) % 256];
        for (i = 1; i < len; i++)
        {
            h = table_random_bytes[h ^ x[i]];
        }
        hh[j] = h;
        hash = hash << (8 * j) | h;
    }

    for(int i = 0; i < 8; i++)
    {
        printf("%s", binary_fmt(hh[i], binary_print_table, 8));
    }
    printf("\n%s\n", binary_fmt(hash, binary_print_table, 64));
}

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

int main()
{
    unsigned char* key = strdup("Hello");

    //size_t len = strlen(&key);
    char* hex = malloc(sizeof(char) * 8);

    Pearson_64_char(key, 4);

    return 1;
}
