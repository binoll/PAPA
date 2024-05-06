#include <stdio.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>

void MyScatter(MPI_Comm comm, int size_message)
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0)
    {
        char send_buf[size * size_message];
        for (int i = 0; i < size; ++i)
        {            
            for (int j = 0; j < size_message; ++j)
            {
                if (j == size_message - 1)
                {
                    send_buf[i * size_message + j] = '\0';
                    break;
                }
                send_buf[i * size_message + j] = 'a';
            }
        }
        for (int i = 0; i < size; ++i)
        {
            MPI_Request request;
            MPI_Isend(&(send_buf[i * size_message]), size_message, MPI_CHAR, i, 0, comm, &request);
        }
        char recv_buf[size_message];
        MPI_Request request_recv;
        MPI_Status status_recv;
        MPI_Irecv(recv_buf, size_message, MPI_CHAR, 0, 0, comm, &request_recv);
        MPI_Wait(&request_recv, &status_recv);
        printf("Rank 0 recv buf: %s\n", recv_buf);
    }
    else
    {
        char recv_buf[size_message];
        MPI_Request request_recv;
        MPI_Status status_recv;
        MPI_Irecv(recv_buf, size_message, MPI_CHAR, 0, 0, comm, &request_recv);
        MPI_Wait(&request_recv, &status_recv);
        printf("Rank %d recv buf: %s\n", rank, recv_buf);
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
    MyScatter(MPI_COMM_WORLD, size_message);
    end_time_my = MPI_Wtime();
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    char send_buf[size_message * size];
    if (rank == 0)
    {        
        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < size_message; ++j)
            {
                if (j == size_message - 1)
                {
                    send_buf[i * size_message + j] = '\0';
                    break;
                }
                send_buf[i * size_message + j] = 'a';
            }
        }
    }
    char recv_buf[size_message];

    start_time = MPI_Wtime();
    MPI_Scatter(send_buf, size_message, MPI_CHAR, recv_buf, size_message, MPI_CHAR, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    if (rank == 0)
    {
        printf("Time my function: %f\n", end_time_my - start_time_my);
        printf("MPI_Scatter time: %f\n", end_time - start_time);
    }
    MPI_Finalize();
    return 0;
}