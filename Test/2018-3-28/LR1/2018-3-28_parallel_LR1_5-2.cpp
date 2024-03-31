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

    if (rank == 0)
    {
        char* buf = new char;
        for (int i = 1; i < n; i++)
        {
            MPI_Recv(buf, 1, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);
            printf("Proc with rank 0 take message from proc with rank=%d: '%c'\n", i, *buf);
        }
    }
    else
    {
        char send = char(int('glushak') + rank);
        MPI_Send(&send, 1, MPI_CHAR, MASTER_RANK, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}