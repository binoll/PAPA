#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define MSG_SIZE 512

int main(int argc, char* argv[])
{
    int rank, size, tag = 777;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status stat[size];
    MPI_Request req[size];
    int snd[size];
    int rcv[size];

    for (int i = 0; i < size; ++i) {
        snd[i] = rank * size + i;
        MPI_Isend(&snd[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD, &req[i]);
    }

    for (int i = 0; i < size; ++i) {
        MPI_Recv(&rcv[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD, &stat[0]);
    }
    MPI_Waitall(size, req, stat);

    for (int i = 0; i < size; ++i) {
        printf("%i ", snd[i]);
    }
    printf("\t");

    for (int i = 0; i < size; ++i) {
        printf("%i ", rcv[i]);
    }
    printf("\n");


    MPI_Finalize();
}





