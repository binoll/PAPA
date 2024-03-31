#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define MSG_SIZE 512

int main(int argc, char* argv[])
{
    double start, finish;
    int rank, src, dest, tag = 777;
    int len = (int)strlen(argv[1]);
    char *rcv = (char*)malloc(len * sizeof(char));
    
    MPI_Status stat;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        src = 1; dest = 1;
        start = MPI_Wtime();
        MPI_Send(argv[1], len, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
        MPI_Recv(rcv, len, MPI_CHAR, src, tag, MPI_COMM_WORLD, &stat);
        finish = MPI_Wtime();
        printf("In %i rank: %s\n", rank, rcv);
        printf("That took %f seconds\n", (finish - start) / 2);
    }
    else if (rank == 1) {
        src = 0; dest = 0;
        start = MPI_Wtime();
        MPI_Recv(rcv, len, MPI_CHAR, src, tag, MPI_COMM_WORLD, &stat);
        MPI_Send(argv[1], len, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
        finish = MPI_Wtime();
        printf("In %i rank: %s\n", rank, rcv);
        printf("That took %f seconds\n", (finish - start) / 2);
    }
    MPI_Finalize();
}
