#include <stdio.h>   // printf()
#include <mpi.h>     // MPI functions
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
    int i = 0;
    while (i < size)
    {
        A[i] = rand() % 10 + rank;
        i++;
    }

    int B[size];
    i = 0;
    while (i < size)
    {
        B[i] = rand() % 10 + i*rank;
        i++;
    }

 

    int recv_buf[size * size];
    MPI_Gather(A, size, MPI_INT, recv_buf, size, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        i = 0;
        printf("Matrix A:\n");
        while (i < size)
        {
            int j = 0;
            while (j < size)
            {
                printf("%d ", recv_buf[i * size + j]);
                j++;
            }
            i++;
            printf("\n");
        }
        printf("\n");
    }

    MPI_Gather(B, size, MPI_INT, recv_buf, size, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        i = 0;
        printf("Matrix B:\n");
        while (i < size)
        {
            int j = 0;
            while (j < size)
            {
                printf("%d ", recv_buf[i * size + j]);
                j++;
            }
            i++;
            printf("\n");
        }
        printf("\n");
    }

    int B_tr[size];

    MPI_Alltoall(B, 1, MPI_INT, B_tr, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Gather(B_tr, size, MPI_INT, recv_buf, size, MPI_INT, 0, MPI_COMM_WORLD);
    // сейчас в recv_buf матрица транспонированная!!!!!!!!!!!!!!!!!!!!!!!!!
    if (rank == 0)
    {
        i = 0;
        printf("Matrix B transp:\n");
        while (i < size)
        {
            int j = 0;
            while (j < size)
            {
                printf("%d ", recv_buf[i * size + j]);
                j++;
            }
            i++;
            printf("\n");
        }
        printf("\n");

        MPI_Bcast(recv_buf, size * size, MPI_INT, 0, MPI_COMM_WORLD);
    }
    else
    {
        MPI_Bcast(recv_buf, size * size, MPI_INT, 0, MPI_COMM_WORLD);
    }

    
    double start_time, end_time;
    start_time = MPI_Wtime();
    int C[size];
    i = 0;
    while (i < size)
    {
        int j = 0;
        C[i] = 0;
        while (j < size)
        {
            C[i] += A[j] * recv_buf[i * size + j];
            j++;
        }
        i++;
    }
    end_time = MPI_Wtime();
    MPI_Gather(C, size, MPI_INT, recv_buf, size, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (rank == 0)
    {
        i = 0;
        printf("Matrix C:\n");
        while (i < size)
        {
            int j = 0;
            while (j < size)
            {
                printf("%d ", recv_buf[i * size + j]);
                j++;
            }
            i++;
            printf("\n");
        }
        printf("\n");

    }
    if (rank == 0)
    {
        printf("Time: %f\n", end_time - start_time);
    }
   
    MPI_Finalize();
    return 0;
}