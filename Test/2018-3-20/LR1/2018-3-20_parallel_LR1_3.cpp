#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#define MAX_MESSAGE_SIZE 10000000

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int procs, rank, namelen, count, source, tag;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

    double start, end;

    int* bufsend = new int[MAX_MESSAGE_SIZE];
    for (int i = 0; i < MAX_MESSAGE_SIZE; i++)
        bufsend[i] = i;

    if (rank == 0) 
    {

        int* bufrecv = new int[MAX_MESSAGE_SIZE];

        start = MPI_Wtime();

        if (MPI_Send(bufsend, MAX_MESSAGE_SIZE, MPI_INT, 1, 1, MPI_COMM_WORLD))
            fprintf(stderr, "MPI_SEND ERROR\n");
        if (MPI_Recv(bufrecv, MAX_MESSAGE_SIZE, MPI_INT, 1, 1, MPI_COMM_WORLD, &status))
            fprintf(stderr, "MPI_RECV ERROR!\n");

        end = MPI_Wtime();

        printf("Rank: %d time: %f\n", rank, ((end - start) / 2) * 1000);

        delete[] bufrecv;
    }
    else if (rank == 1) 
    {

        int* bufrecv = new int[MAX_MESSAGE_SIZE];

        start = MPI_Wtime();

        if (MPI_Recv(bufrecv, MAX_MESSAGE_SIZE, MPI_INT, 0, 1, MPI_COMM_WORLD, &status))
            fprintf(stderr, "MPI_RECV ERROR!\n");
        if (MPI_Send(bufsend, MAX_MESSAGE_SIZE, MPI_INT, 0, 1, MPI_COMM_WORLD))
            fprintf(stderr, "MPI_SEND ERROR\n");

        end = MPI_Wtime();

        printf("Rank: %d time: %f\n", rank, ((end - start) / 2) * 1000);

        delete[] bufrecv;
    }

    MPI_Finalize();

}