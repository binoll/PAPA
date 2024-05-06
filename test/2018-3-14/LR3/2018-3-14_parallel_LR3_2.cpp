
/*
    2. Разработать параллельную программу, демонстрирующую использование производных типов данных совместно
        с операциями коллективного взаимодействия на примере квадратной матрицы произвольного размера.
        Размер квадратной матрицы должен задаваться в качестве входного параметра.
        Элементы матрицы должны генерироваться случайным образом.
    
*/

#include "lr3.h"

#define ROOT_RANK 0

int main(int argc, char* argv[])
{
    int rank, n, size, sum, i, j;
    long int sizeofint;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    getopt_s(argc, argv, n, &size);

    int Matrix_A[size][size];
    int Matrix_B[size][size];
    int Matrix_C[size][size];
    int row_A[size];
    int col_B[size];

    if (rank == ROOT_RANK) {
        srand(time(NULL));
        for(i = 0; i < size; i++) {
            for(j = 0; j < size; j++) {
                Matrix_A[i][j] = rand() % 10;
                Matrix_B[i][j] = rand() % 10;
                Matrix_C[i][j] = 0;
            }
        }
    }

    MPI_Datatype row, column, col, diag, matrix, xpose; 
    MPI_Type_extent(MPI_INT, &sizeofint);
    MPI_Type_contiguous(size, MPI_INT, &row);
    MPI_Type_vector(size, 1, size, MPI_INT, &column);
    MPI_Type_create_resized(column, 0, sizeofint, &col);
    MPI_Type_hvector(size, 1, sizeofint, column, &xpose);
    MPI_Type_vector(size, 1, 1, row, &matrix);
    MPI_Type_create_resized(MPI_INT, 0, sizeofint * (size + 1), &diag);

    MPI_Type_commit(&row);
    MPI_Type_commit(&column);
    MPI_Type_commit(&col);
    MPI_Type_commit(&diag);
    MPI_Type_commit(&matrix);
    MPI_Type_commit(&xpose);

    // ********************************** MPI_Func **********************************

    // MPI_Sendrecv(Matrix_A, 1, xpose, ROOT_RANK, 0, Matrix_B, 1, matrix, ROOT_RANK, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    // MPI_Sendrecv(Matrix_A, 1, matrix, ROOT_RANK, 0, Matrix_C, 1, matrix, ROOT_RANK, 0, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
    // if (MAX_RPINT > size && rank == ROOT_RANK) { printf("Matrix B\n"); print_matrix(size, size, Matrix_B); }
    // if (MAX_RPINT > size && rank == ROOT_RANK) { printf("Matrix C\n"); print_matrix(size, size, Matrix_C); }

    // if (MAX_RPINT > size && rank == ROOT_RANK) { printf("Matrix A\n"); print_matrix(size, size, Matrix_A); }
    MPI_Scatter(Matrix_A, 1, row, row_A, 1, row, ROOT_RANK, MPI_COMM_WORLD);
    // printf_vector(rank, size, row_A);

    // MPI_Barrier(MPI_COMM_WORLD);

    // if (MAX_RPINT > size && rank == ROOT_RANK) { printf("Matrix B\n"); print_matrix(size, size, Matrix_B); }
    MPI_Scatter(Matrix_B, 1, col, col_B, 1, row, ROOT_RANK, MPI_COMM_WORLD);
    // printf_vector(rank, size, col_B);

    sum = 0;
    for (i = 0; i < size; i++) {
        sum += row_A[i] * col_B[i];
    }

    MPI_Gather(&sum, 1, MPI_INT, Matrix_C, 1, diag, ROOT_RANK, MPI_COMM_WORLD);

    // ********************************** MPI_Func **********************************

    if (MAX_RPINT > size && rank == ROOT_RANK) {
        printf("Matrix A\n"); print_matrix(size, size, Matrix_A);
        printf("Matrix B\n"); print_matrix(size, size, Matrix_B);
        printf("Matrix C\n"); print_matrix(size, size, Matrix_C);
    }

    MPI_Type_free(&row);
    MPI_Type_free(&column);
    MPI_Type_free(&col);
    MPI_Type_free(&diag);
    MPI_Type_free(&matrix);
    MPI_Type_free(&xpose);
    
    MPI_Exit(EXIT_SUCCESS);
}