%%writefile task1_6.c
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

void my_MPI_Alltoall(MPI_Comm comm, int size)
{
    int rank, tag = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int send_buf[size];
    int i = 0;
    while (i < size)
    {
        send_buf[i] = rank;
        i++;
    }

    i = 0;
    while (i < size)
    {
        MPI_Request request;
        MPI_Isend(&send_buf[i], 1, MPI_INT, i, tag, comm, &request);
        i++;
    }

    int recv_buf[size];
    i = 0;
    while (i < size)
    {
        MPI_Request request_recv;
        MPI_Status status_recv;
        MPI_Irecv(&recv_buf[i], 1, MPI_INT, i, tag, comm, &request_recv);
        MPI_Wait(&request_recv, &status_recv);
        i++;
    }

    i = 0;
    //printf("Rank %d recv: ", rank);
    while (i < size)
    {
       // printf("%d ", recv_buf[i]);
        i++;
    }
    //printf("\n");
}


int main(int argc, char** argv)
{

    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double start_time, end_time, start_time_my, end_time_my;
    start_time_my = MPI_Wtime();
    my_MPI_Alltoall(MPI_COMM_WORLD, size);
    end_time_my = MPI_Wtime();

    int send_buf[size];
    int i = 0;
    while (i < size)
    {
        send_buf[i] = rank;
        i++;
    }
    int recv_buf[size];
    start_time = MPI_Wtime();
    MPI_Alltoall(send_buf, 1, MPI_INT, recv_buf, 1, MPI_INT, MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    if (rank == 0)
    {
        printf("My MPI_Alltoall time: %f\n", end_time_my - start_time_my);
        printf("MPI_Alltoall time: %f\n", end_time - start_time);
    }
    MPI_Finalize();
    return 0;
}