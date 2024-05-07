#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define MSG_SIZE 512

int main(int argc, char* argv[])
{
    int rank, perv, size, next, tag = 777;
    int len = (int)strlen(argv[1]);
    char *rcv = (char*)malloc(len * sizeof(char));;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status stat;

    perv = rank - 1;
    if (perv < 0)
        perv = size - 1;
    next = rank + 1;
    if (next > size - 1)
        next = 0;

    if (rank == 0) {
        MPI_Send(argv[1], len, MPI_CHAR, next, tag, MPI_COMM_WORLD);
    }
    else {
        MPI_Recv(rcv, len, MPI_CHAR, perv, tag, MPI_COMM_WORLD, &stat);
        printf("In %i rank: %s\n", rank, rcv);
        MPI_Send(argv[1], len, MPI_CHAR, next, tag, MPI_COMM_WORLD);
        free(rcv);
    }
    MPI_Finalize();
}