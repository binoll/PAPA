#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int n, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

    char* recv = new char;
    char* buf = new char;

    int destination, source;

    if (rank == n - 1)
        destination = 0;
    else
        destination = rank + 1;

    if (rank == 0)
        source = n - 1;
    else
        source = rank - 1;

    *buf = char(rank);

    MPI_Sendrecv(buf, 1, MPI_CHAR, destination, 1, recv, 1, MPI_CHAR, source, 1, MPI_COMM_WORLD, &status);
    printf("rank=%d, recv from %d: '%d', send to %d: '%d'\n", rank, source, *recv, destination, *buf);

    MPI_Finalize();
    return 0;
}