typedef struct _matrix_t_
{
	int num_rows;
	int num_columns;
	int allocated_rows;
	int allocated_columns;
	int** matrix
} matrix_t;

matrix_t* matrix_init();
matrix_t* matrix_create(int num_rows, int num_colums);

void matrix_destroy(matrix_t* matrix);
void matrix_delete(int** matrix);

void matrix_insert_equation(matrix_t* matrix, int* equation);
void matrix_delete_equation(int row_number);

void print_matrix(matrix_t* matrix);

// Basic Matrix row operations.
void matrix_add_rows(matrix_t* matrix, int row_one, int row_two);
void matrix_subtract_rows(matrix_t* matrix, int row_one, int row_two);
void matrix_multiply_scalar_row(matrix_t* matrix, int scalar, int row);

// Provided that they are the same size (have the same number of rows and the same number of columns).
// Two matrices can be added or subtracted element by element.
// The rule for matrix multiplication:
// The two matrices can be multiplied only when the number of columns in the first
// equals the number of rows in the second.

matrix_t* multipy_matrices(matrix_t* matrix_one, matrix_t* matrix_two);


/*
	Row Reduction algorithm:
	1. Begin with the left most column. Mark curent column as pivot column.
	2. Select a non-zero entry in the column as pivot position/element.
	3. Use row replacement operation to create zeros in all other entries in the pivot column.

	4. Cover or ignore the Row containing the pivot position and cover all rows, if any, above the pivot.
		 Starting from the right most pivot, work up the the left most, creating zeros above the pivot.
		 If the pivot is not 1, make it one by using a scalar operation.
*/
