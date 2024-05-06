#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define MASTER_RANK 0

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int n, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Status status;
    MPI_Request request;

    char* recv = new char;
    char* send = new char;
    *send = char(int('a') + rank);

    for (int i = 0; i < n; i++)
    {
        if (i == rank)
            continue;
        MPI_Isend(send, 1, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
        MPI_Recv(recv, 1, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);
        printf("rank=%d from rank=%d, recv='%c'\n", rank, i, *recv);
    }
    MPI_Finalize();
    return 0;
}