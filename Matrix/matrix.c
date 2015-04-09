#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <stddef.h>

#include "matrix.h"

#define INVALID INT_MIN

matrix_t* matrix_init()
{
  matrix_t* matrix = malloc(sizeof(matrix_t));
  matrix->num_rows = 0;
  matrix->num_columns = 0;

  matrix->allocated_rows = 1;
  matrix->allocated_columns = 1;

  matrix->matrix = malloc(sizeof(int**) * allocated_columns);
  matrix->matrix[num_columns] = malloc(sizeof(int*) * allocated_rows);
  matrix->matrix[num_columns][num_rows] = INVALID;
}

matrix_t* matrix_create(int num_rows, int num_colums)
{
  matrix_t* matrix = malloc(sizeof(matrix_t));
  matrix->num_rows = num_rows;
  matrix->num_columns = num_columns;

  matrix->allocated_rows = num_rows;
  matrix->allocated_columns = num_columns;

  matrix->matrix = malloc(sizeof(int**) * allocated_columns);
  matric->matrix[0] = malloc(sizeof(int*) * allocated_rows);
}

void matrix_destroy(matrix_t* matrix)
{
  for(int i = 0; i < num_colums; i++)
  {
    for(int j = 0; j < num_rows; j++)
    {
      free(matrix[i][j]);
    }

    free(matrix[i])
  }

  free(matrix);

  return;
}

void matrix_delete(int** matrix)
{

}

void matrix_insert_equation(matrix_t* matrix, int* equation)
{
  if(matrix->num_columns == matrix->allocated_columns)
  {
    matrix->allocated_columns *= 2;
    matrix = realloc(matrix, sizeof(int*) * matrix->allocated_columns);
  }
  if(matrix->num_rows == matrix->allocated_rows)
  {
    matrix->allocated_rows *= 2;
    matrix = realloc(matrix, sizeof(int*) * matrix->allocated_columns);
  }

}

void matrix_delete_equation(int row_number)
{

}

// Basic Matrix row operations.
void matrix_add_rows(matrix_t* matrix, int row_one, int row_two)
{

}

void matrix_subtract_rows(matrix_t* matrix, int row_one, int row_two)
{

}

void matrix_multiply_scalar_row(matrix_t* matrix, int scalar, int row)
{

}


/*
Matrix Multiplication:
 Provided that they are the same size (have the same number of rows and the same number of columns).
 Two matrices can be added or subtracted element by element.
 The rule for matrix multiplication:
 The two matrices can be multiplied only when the number of columns in the first
 equals the number of rows in the second.
*/

matrix_t* multipy_matrices(matrix_t* matrix_one, matrix_t* matrix_two)
{

}

/*
 Matrices represent linear transformations, that is, generalizations of linear functions such as f(x) = 4x.
 The rotation of vectors in three dimensional space is a linear transformation.
 It can be represented by a rotation matrix R:
  If v is a column vector (a matrix with only one column) describing the position of a point in space,
  The product Rv is a column vector describing the position of that point after a rotation.
*/
