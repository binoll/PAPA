#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
    int opt, rank, size, i, j;
    long int size_of_int;
    int matrix_size;
    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
        case 's':
            matrix_size = atoi(optarg);
            break;
        }
    }

    int A[matrix_size][matrix_size];
    int B[matrix_size][matrix_size];

    for (i = 0; i < matrix_size; i++)
    {
        for (j = 0; j < matrix_size; j++)
            A[i][j] = rand()%10;
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Datatype column, xpose;

    MPI_Type_extent(MPI_INT, &size_of_int);
    MPI_Type_vector(matrix_size, 1, matrix_size, MPI_INT, &column);
    MPI_Type_hvector(matrix_size, 1, size_of_int, column, &xpose);
    MPI_Type_commit(&xpose);
    if (rank == 0)
    {
        MPI_Bcast(A, 1, xpose, 0, MPI_COMM_WORLD);
        printf("Matrix A from rank 0\n");
        for  (i = 0; i < matrix_size; i++)
        {
            for (j = 0; j < matrix_size; j++)
                printf("%d ", A[i][j]);
            printf("\n");
        }
        MPI_Bcast(A, 1, xpose, 0, MPI_COMM_WORLD);
    }
    else
    {
        printf("From rank: %d before BCast\n", rank);
        for (i = 0; i < matrix_size; ++i)
        {
            for (j = 0; j < matrix_size; ++j)
                printf("%d ", B[i][j]);
            printf("\n");
        }

        MPI_Bcast(B, 1, xpose, 0, MPI_COMM_WORLD);
        
        printf("From rank: %d after BCast\n", rank);
        for (i = 0; i < matrix_size; ++i)
        {
            for (j = 0; j < matrix_size; ++j) 
                printf("%d ", B[i][j]);
            printf("\n");
        }
    }
    MPI_Finalize();
    return 0;
}