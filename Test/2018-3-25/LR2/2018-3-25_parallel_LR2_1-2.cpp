#include <stdio.h>   // printf()
#include <mpi.h>     // MPI functions
#include <unistd.h>
#include <stdlib.h>

void my_MPI_Bcast(MPI_Comm comm, int size_message)
{
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    int tag = 0;
    MPI_Request request;
    if (rank == 0)
    {
        char* send_buf = NULL;
        send_buf = (char*)malloc(size_message * sizeof(char));
        int i = 0;
        while (i < size_message)
        {
            if (i == size_message - 1)
            {
                send_buf[i] = '\0';
                break;
            }
            send_buf[i] = 'a';
            i++;
        }
        printf("Buf in rank %d: %s\n", rank, send_buf);

        i = 1;
        while (i < size)
        {
            MPI_Isend(send_buf, size_message, MPI_CHAR, i, tag, comm, &request);
            i++;
        }
        free(send_buf);
        i = 1;
        while (i < size)
        {
            int buf = 0;
            MPI_Status status;
            MPI_Irecv(&buf, 1, MPI_INT, i, tag, comm, &request);
            MPI_Wait(&request, &status);
            //printf("Process %d successful recv\n", i);
            i++;
        }
    }
    else
    {
        char* recv_buf = NULL;
        recv_buf = (char*)malloc(size_message * sizeof(char));
        MPI_Status status;
        MPI_Irecv(recv_buf, size_message, MPI_CHAR, 0, tag, comm, &request);
        MPI_Wait(&request, &status);
        MPI_Isend(&rank, 1, MPI_INT, 0, tag, comm, &request);
        printf("Recv from 0 buf: %s\n", recv_buf);
        free(recv_buf);
    }
}


int main(int argc, char** argv)
{
    int opt;
    int size_message;
    while ((opt = getopt(argc, argv, "n:")) != -1)
    {
        switch (opt)
        {
        case 'n':
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
    my_MPI_Bcast(MPI_COMM_WORLD, size_message);
    end_time_my = MPI_Wtime();

    char* send_buf = NULL;
    send_buf = (char*)malloc(size_message * sizeof(char));
    int i = 0;
    while (i < size_message)
    {
        if (i == size_message - 1)
        {
            send_buf[i] = '\0';
            break;
        }
        send_buf[i] = 'a';
        i++;
    }
    start_time = MPI_Wtime();
    MPI_Bcast(send_buf, size_message, MPI_CHAR, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    free(send_buf);
    if (rank == 0)
    {
        printf("Time my function: %f\n", end_time_my - start_time_my);
        printf("Time MPI function: %f\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}