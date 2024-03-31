#include <stdio.h>   // printf()
#include <mpi.h>     // MPI functions
#include <unistd.h>
#include <stdlib.h>

void my_MPI_Barrier(MPI_Comm comm)
{
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == 0)
    {
        int i = 1, buf = 0, tag = 0, sum = 0;
        MPI_Request request[2];
        MPI_Status status[2];
        while (i < size)
        {
            MPI_Irecv(&buf, 1, MPI_INT, i, tag, comm, &request[0]);
            MPI_Wait(&request[0], &status[0]);
            sum += buf;
            i++;
        }
        i = 1;
        while (i < size)
        {
            MPI_Isend(&rank, 1, MPI_INT, i, tag, comm, &request[1]);
            i++;
        }
        printf("Synchronization successful. Sum rank:%d\n", sum);
    }
    else
    {
        int tag = 0, buf = 1;
        MPI_Request request[2];
        MPI_Status status;
        MPI_Isend(&rank, 1, MPI_INT, 0, tag, comm, &request[0]);
        MPI_Irecv(&buf, 1, MPI_INT, 0, tag, comm, &request[1]);
        MPI_Wait(&request[1], &status);
    }
}


int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    double start_time_my, end_time_my, start_time, end_time;
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    start_time_my = MPI_Wtime();
    my_MPI_Barrier(MPI_COMM_WORLD);
    end_time_my = MPI_Wtime();

    start_time = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    if (rank == 0)
    {
        printf("Time my function:%f\n", end_time_my - start_time_my);
        printf("Time MPI function:%f\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
