#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "mpi.h"
#define SIZE_BUF 1024

int main(int argc, char *argv[]) {
    int procs, rank, namelen, count, source, tag;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    char* sendbuf1 ="message1";
    char* sendbuf2 = "message2";
    char* recvbuf1 = (char*)malloc(SIZE_BUF*sizeof(char));
    char* recvbuf2 = (char*)malloc(SIZE_BUF*sizeof(char));
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status[2];

    double start, end;
    start = MPI_Wtime();
    if (rank == 0) {
        MPI_Send(sendbuf1, strlen(sendbuf1), MPI_CHAR, 1, tag, MPI_COMM_WORLD);
        MPI_Recv(recvbuf1, SIZE_BUF*sizeof(char), MPI_CHAR, 1, tag, MPI_COMM_WORLD, &status[0]);
        end = MPI_Wtime();
        printf("Blocked messege by rank 1: %s, time: %f\n", recvbuf1, (end-start)/2);
    }
    else if (rank == 1) {
        MPI_Recv(recvbuf2, SIZE_BUF*sizeof(char), MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status[1]);
        MPI_Send(sendbuf2, strlen(sendbuf2), MPI_CHAR, 0, tag, MPI_COMM_WORLD);
        end = MPI_Wtime();
        printf("Blocked messege by rank 0: %s, time: %f\n", recvbuf2, (end-start)/2);
    }
    MPI_Request requests[2];
    start = MPI_Wtime();
    if (rank == 0) {
        MPI_Isend(sendbuf1, strlen(sendbuf1), MPI_CHAR, 1, tag, MPI_COMM_WORLD, &requests[0]);
        MPI_Irecv(recvbuf1, SIZE_BUF*sizeof(char), MPI_CHAR, 1, tag, MPI_COMM_WORLD, &requests[1]);
        end = MPI_Wtime();
        printf("Unblocked messege by rank 1: %s, time: %f\n", recvbuf1, (end-start)/2);
    }
    else if (rank == 1) {
        MPI_Isend(sendbuf2, strlen(sendbuf2), MPI_CHAR, 0, tag, MPI_COMM_WORLD, &requests[0]);
        MPI_Irecv(recvbuf2, SIZE_BUF*sizeof(char), MPI_CHAR, 0, tag, MPI_COMM_WORLD, &requests[1]);
        end = MPI_Wtime();
        printf("Unblocked messege by rank 0: %s, time: %f\n", recvbuf2, (end-start)/2);
    }
    MPI_Waitall(2, requests, status);
    MPI_Finalize();
}
