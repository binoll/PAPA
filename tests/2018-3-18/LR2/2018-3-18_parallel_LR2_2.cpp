#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

void multi_matrix(const int* row, const int* colms, int* result_str, int size) {
    for (int i = 0; i < size; ++i) {
        result_str[i] = 0;
        for (int j = 0; j < size; ++j) {
            result_str[i] += row[j] * colms[i * size + j];
        }
    }
}

void print_matrices(int size){
    int i, j;
    printf("Matrix A:\n");
    for (i = 0; i < size; ++i) {
        for (j = 0; j < size; ++j)
            printf("%4d ", i * size + j + 1);
        puts("");
    }
    printf("\nMatrix B:\n");
    for (i = 0; i < size; ++i) {
        for (j = 0; j < size; ++j)
            printf("%4d ", j * size + i + 1);
        puts("");
    }
    puts("");
}

int main(int argc, char* argv[]) {
    int rank, size, i, j;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int* row = (int*)malloc(size * sizeof(int));
    int* col = (int*)malloc(size * sizeof(int));
    int* result_str = (int*)malloc(size * sizeof(int));
    int* recv = (int*)malloc(size * size * sizeof(int));

    if(rank == 0){
        print_matrices(size);
    }

    for (i = 0; i < size; i++) {
        row[i] = rank * size + i + 1;
        col[i] = rank * size + i + 1;
    }
    MPI_Allgather(col,size,MPI_INT,
                 recv,size, MPI_INT,
                 MPI_COMM_WORLD);
    
    multi_matrix(row, recv, result_str, size);

    MPI_Gather(result_str, size, MPI_INT,
               recv,size,MPI_INT,
               0,MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    if(rank == 0){
        sleep(1);
        printf("Matrix A x B:\n");
        for (i = 0, j = 1; i < size * size; i++, j++) {
            printf("%4d ", recv[i]);
            if(j % size == 0)
                printf("\n");
        }
    }


    free(row);
    free(col);
    free(result_str);
    free(recv);
    MPI_Finalize();
}