#include <stdio.h>
#include "mpi.h"   
#include <unistd.h>
#include <stdlib.h>

void MyAllgather(MPI_Comm comm, int size_message)
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int send_buf[size_message];    
    for (int i = 0; i < size_message; ++i)
    {
        send_buf[i] = rank;
    }
    for (int i = 0; i < size; ++i)
    {
        MPI_Request request;
        MPI_Isend(send_buf, size_message, MPI_INT, i, 0, comm, &request);
    }
    int recv_buf[size_message * size];
    for (int i = 0; i < size; ++i)
    {
        MPI_Request request_recv;
        MPI_Status status_recv;
        MPI_Irecv(&recv_buf[i * size_message], size_message, MPI_INT, i, 0, comm, &request_recv);
        MPI_Wait(&request_recv, &status_recv);
    }
    for (int i = 0; i < size; ++i)
    {        
        printf("Rank %d recv from rank %d: ", rank, i);
        for (int j = 0; j < size_message; j++)
        {
            printf("%d ", recv_buf[i * size_message + j]);       
        }
        printf("\n");
    }
}

int main(int argc, char** argv)
{
    int opt;
    int size_message = 0;
    while ((opt = getopt(argc, argv, "s:")) != -1)
    {
        switch (opt)
        {
        case 's':
            size_message = atoi(optarg);
            break;
        default:
            return 1;
        }
    }

    MPI_Init(&argc, &argv);
    double start_time, end_time, start_time_my, end_time_my;
    start_time_my = MPI_Wtime();
    MyAllgather(MPI_COMM_WORLD, size_message);
    end_time_my = MPI_Wtime();

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int send_buf[size_message];
    for (int i = 0; i < size_message; ++i)
    {
        send_buf[i] = rank;
    }
    int recv_buf[size * size_message];
    start_time = MPI_Wtime();
    MPI_Allgather(send_buf, size_message, MPI_INT, recv_buf, size_message, MPI_INT, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    if (rank == 0)
    {
        printf("Time my function: %f\n", end_time_my - start_time_my);
        printf("MPI_Allgather time: %f\n", end_time - start_time);
    }
    MPI_Finalize();
    return 0;
}