#include <stdio.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv)
{
    srand(time(0));
    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int A[size];    
    for (int i = 0; i < size; ++i)
    {
        A[i] = rand() % 10 + rank;
    }

    int B[size];
    for (int i = 0; i < size; ++i)
    {
        B[i] = rand() % 10 + i*rank;
    }

    int recv_buf[size * size];
    MPI_Gather(A, size, MPI_INT, recv_buf, size, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        printf("Matrix A:\n");
        for (int i = 0; i < size; ++i)
        {            
            for (int j = 0; j < size; ++j)
            {
                printf("%d ", recv_buf[i * size + j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    MPI_Gather(B, size, MPI_INT, recv_buf, size, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        printf("Matrix B:\n");
        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < size; ++j)
            {
                printf("%d ", recv_buf[i * size + j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    int B_tr[size];

    MPI_Alltoall(B, 1, MPI_INT, B_tr, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Gather(B_tr, size, MPI_INT, recv_buf, size, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank == 0)
    {
        printf("Matrix B transp:\n");
        for (int i = 0; i < size; ++i)
        {            
            for (int j = 0; j < size; ++j)
            {
                printf("%d ", recv_buf[i * size + j]);
            }
            printf("\n");
        }
        printf("\n");

        MPI_Bcast(recv_buf, size * size, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Bcast(recv_buf, size * size, MPI_INT, 0, MPI_COMM_WORLD);
    }
    
    int C[size];
    for (int i = 0; i < size; ++i)
    {
        C[i] = 0;
        for (int j = 0; j < size; ++j)
        {
            C[i] += A[j] * recv_buf[i * size + j];
        }
    }
    MPI_Gather(C, size, MPI_INT, recv_buf, size, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank == 0)
    {
        printf("Matrix C:\n");
        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < size; ++j)
            {
                printf("%d ", recv_buf[i * size + j]);
            }
            printf("\n");
        }
        printf("\n");

    }
   
    MPI_Finalize();
    return 0;
}
