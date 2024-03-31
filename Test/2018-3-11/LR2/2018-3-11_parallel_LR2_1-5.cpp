%%writefile task1_5.c
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

void my_MPI_Allgather(MPI_Comm comm, int size_message)
{
    int rank, size, tag = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int send_buf[size_message];
    int i = 0;
    while (i < size_message)
    {
        send_buf[i] = rank;
        i++;
    }
    i = 0;
    while (i < size)
    {
        MPI_Request request;
        MPI_Isend(send_buf, size_message, MPI_INT, i, tag, comm, &request);
        i++;
    }

    i = 0;
    int recv_buf[size_message * size];
    while (i < size)
    {
        MPI_Request request_recv;
        MPI_Status status_recv;
        MPI_Irecv(&recv_buf[i * size_message], size_message, MPI_INT, i, tag, comm, &request_recv);
        MPI_Wait(&request_recv, &status_recv);
        i++;
    }

    i = 0;
    while (i < size)
    {
        int j = 0;
       // printf("Rank %d recv from rank %d: ", rank, i);
        while (j < size_message)
        {
            //printf("%d ", recv_buf[i * size_message + j]);
            j++;
        }
        //printf("\n");
        i++;
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
    my_MPI_Allgather(MPI_COMM_WORLD, size_message);
    end_time_my = MPI_Wtime();

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int send_buf[size_message];
    int i = 0;
    while (i < size_message)
    {
        send_buf[i] = rank;
        i++;
    }
    int recv_buf[size * size_message];
    start_time = MPI_Wtime();
    MPI_Allgather(send_buf, size_message, MPI_INT, recv_buf, size_message, MPI_INT, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    if (rank == 0)
    {
        printf("My MPI_Allgather time: %f\n", end_time_my - start_time_my);
        printf("MPI_Allgather time: %f\n", end_time - start_time);
    }
    MPI_Finalize();
    return 0;
}