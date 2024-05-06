#include <stdio.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>

void MyAlltoall(MPI_Comm comm, int size)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int send_buf[size];   

    int recv_buf[size];
    for (int i = 0; i < size; ++i)
    {
        send_buf[i] = rank;
        MPI_Request request;
        MPI_Isend(&send_buf[i], 1, MPI_INT, i, 0, comm, &request);
        MPI_Request request_recv;
        MPI_Status status_recv;
        MPI_Irecv(&recv_buf[i], 1, MPI_INT, i, 0, comm, &request_recv);
        MPI_Wait(&request_recv, &status_recv);
    }
  
    printf("Rank %d recv: ", rank);
    for (int i = 0; i < size; ++i)
    {
        printf("%d ", recv_buf[i]);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double start_time, end_time, start_time_my, end_time_my;

    start_time_my = MPI_Wtime();
    MyAlltoall(MPI_COMM_WORLD, size);
    end_time_my = MPI_Wtime();

    int send_buf[size];

    for (int i = 0; i < size; ++i)
    {
        send_buf[i] = rank;
    }
    int recv_buf[size];

    start_time = MPI_Wtime();
    MPI_Alltoall(send_buf, 1, MPI_INT, recv_buf, 1, MPI_INT, MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    if (rank == 0)
    {
        printf("Time my function: %f\n", end_time_my - start_time_my);
        printf("MPI_Alltoall time: %f\n", end_time - start_time);
    }
    MPI_Finalize();
    return 0;
}