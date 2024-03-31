#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "mpi.h"
#define SIZE_BUF 1024

int main(int argc, char *argv[]) {
    int procs, rank, namelen, count, source, tag;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    char* sendbuf ="message";
    char* recvbuf = (char*)malloc(SIZE_BUF*sizeof(char));
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;

    double start, end;
    start = MPI_Wtime();
    if (rank == 0) {
        MPI_Send(sendbuf, strlen(sendbuf), MPI_CHAR, rank+1, tag, MPI_COMM_WORLD);
    }
    else {
        MPI_Recv(recvbuf, SIZE_BUF*sizeof(char), MPI_CHAR, rank-1, tag, MPI_COMM_WORLD, &status);
        MPI_Send(sendbuf, strlen(sendbuf), MPI_CHAR, rank+1, tag, MPI_COMM_WORLD);
        end = MPI_Wtime();
        printf("Blocked messege by rank%d : %s, time: %f\n",rank, recvbuf, (end-start)/2);
    }

    MPI_Finalize();
}
