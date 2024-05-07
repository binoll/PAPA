#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define MSG_SIZE 512

int main(int argc, char* argv[])
{
    double start, finish;
    int rank, src = 0, dest = 1, tag = 777;
    int len = (int)strlen(argv[1]);
    char *rcv = (char*)malloc(len * sizeof(char));

    MPI_Init(&argc, &argv);
    start = MPI_Wtime();
    MPI_Request request;
    MPI_Status stat;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        MPI_Isend(argv[1], len, MPI_CHAR, dest, tag, MPI_COMM_WORLD, &request);
    }
    else if (rank == 1) {
        MPI_Irecv(rcv, len, MPI_CHAR, src, tag, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &stat);
        finish = MPI_Wtime();
        printf("In %i rank: %s\n", rank, rcv);
        printf("That took %f seconds in proc %i\n", finish - start, rank);
        free(rcv);
    }
    MPI_Finalize();
}


