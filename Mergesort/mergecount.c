/*
 * mergesort.c
 *
 *  Created on: Feb 1, 2015
 *      Author: vyshnav
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long merge(int* array_one, int size_one, int* array_two, int size_two, int* array)
{
    int i = 0;
    int j = 0;
    int k = 0;
    long inversions = 0;

    while(i < size_one && j < size_two)
    {
      if(array_one[i] < array_two[j])
      {
        array[k] = array_one[i];
        i++;
        k++;
      }
      else
      {
        array[k] = array_two[j];
        j++;
        k++;
        inversions += size_one - i;
      }
    }

    while(i < size_one)
    {
      array[k] = array_one[i];
      i++;
      k++;
    }

    while(j < size_two)
    {
      array[k] = array_two[j];
      j++;
      k++;
    }

    for(i = 0; i < size_one; i++)
    {
        array_one[i] = array[i];
    }

    for(j = 0; j < size_two; j++)
    {
        array_two[j] = array[j + size_one];
    }

    free(array);
    
    return inversions;
}

long merge_sort(int* array, int size)
{
  if(size <= 1)
    return 0;

  int i;
  int size_one = size/2;;
  int size_two = size - size_one;
  int* array_one = malloc(sizeof(int) * size_one);
  int* array_two = malloc(sizeof(int) * size_two);

  for(i = 0; i < size_one; i++)
  {
    array_one[i] = array[i];
  }

  for(i = 0; i < size_two; i++)
  {
    array_two[i] = array[size_one + i];
  }

  long inversions;
  inversions = merge_sort(array_one, size_one);
  inversions += merge_sort(array_two, size_two);

  inversions += merge(array_one, size_one, array_two, size_two, array);

  free(array_one);
  free(array_two);

  return inversions;
}

int main()
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    int array_length = 100;
    int* array = malloc(sizeof(int) * array_length);
    int num_lines = 0;

    while ((read = getline(&line, &len, stdin)) != -1)
    {
       if (num_lines == array_length)
       {
          array_length *= 2;
          array = realloc(array, sizeof(int) * array_length);
       }

       array[num_lines] = atoi(line);
       num_lines++;
    }

    free(line);

    long inversions = merge_sort(array, num_lines);

    printf("Number of Inversions: %lu \n", inversions);

    free(array);

    return 0;
}
