#include <stdio.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "time.h"
#include <iostream>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Datatype MATRIX;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    srand(time(NULL));

    int N = 3;
    int opt;
    while ((opt = getopt(argc, argv, "n: :")) != -1)
    {
        switch (opt)
        {
            case 'n':
                N = atoi(optarg);
                break;
            case ' ':
                break;
            default:
                fprintf(stderr, ": %s [-n process_id]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    int matrix[N][N];
    MPI_Type_vector(N, N, N, MPI_INT, &MATRIX);
    MPI_Type_commit(&MATRIX);
    if(rank == 0)
    {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                matrix[i][j] = rand() % 10;

    }
    MPI_Bcast(&matrix[0][0], 1, MATRIX, 0, MPI_COMM_WORLD);

    //================== ПЕЧАТЬ =====================
    sleep(size/(rank+1));
    if(rank != 0)
        printf("[RECV MATRIX] rank=%d:\n", rank);
    else
        printf("[SEND MATRIX] rank=%d:\n", rank);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            printf("%d ", matrix[i][j]);
        printf("\n");
    }
    //================== ПЕЧАТЬ =====================

    MPI_Type_free(&MATRIX);
    MPI_Finalize();
}