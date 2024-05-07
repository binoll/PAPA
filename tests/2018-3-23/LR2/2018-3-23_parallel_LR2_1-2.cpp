#include <stdio.h>  
#include "mpi.h"     
#include <unistd.h>
#include <stdlib.h>

void MyBcast(MPI_Comm comm, int size_message)
{
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    MPI_Request request;
    if (rank == 0)
    {
        char* send_buf = NULL;
        send_buf = (char*)malloc(size_message * sizeof(char));
        for(int i = 0; i < size_message; ++i)
        {
            if (i == size_message - 1)
            {
                send_buf[i] = '\0';
                break;
            }
            send_buf[i] = 'a';
        }
       
        for (int i = 1; i < size; ++i)
        {
            MPI_Isend(send_buf, size_message, MPI_CHAR, i, 0, comm, &request);
        }

        printf("Buf in rank %d: %s\n", rank, send_buf);
        
        free(send_buf);
    }
    else
    {
        char* recv_buf = NULL;
        recv_buf = (char*)malloc(size_message * sizeof(char));
        MPI_Status status;
        MPI_Irecv(recv_buf, size_message, MPI_CHAR, 0, 0, comm, &request);
        MPI_Wait(&request, &status);
        printf("Rank:%d Recv from 0 buf: %s\n", rank, recv_buf);
        free(recv_buf);
    }
}


int main(int argc, char** argv)
{
    int opt;
    int size_message;
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
    double start_time_my, end_time_my, start_time, end_time;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    start_time_my = MPI_Wtime();
    MyBcast(MPI_COMM_WORLD, size_message);
    end_time_my = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);

    char* send_buf = NULL;
    send_buf = (char*)malloc(size_message * sizeof(char));
    for (int i = 0; i < size_message; ++i)
    {
        if (i == size_message - 1)
        {
            send_buf[i] = '\0';
            break;
        }
        send_buf[i] = 'a';
    }

    start_time = MPI_Wtime();
    
    MPI_Bcast(send_buf, size_message, MPI_CHAR, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    free(send_buf);
    if (rank == 0)
    {
        printf("Time my function: %f\n", end_time_my - start_time_my);
        printf("MPI_Bcast time: %f\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}