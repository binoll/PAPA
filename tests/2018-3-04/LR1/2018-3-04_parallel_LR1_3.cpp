#include <stdio.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define SIZE_BUF 1024

int main(int argc, char *argv[]){
    int procs, rank, namelen, count, source, tag;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    
    char* sendbuf1 ="message1";
    char* sendbuf2 = "message2";
    char* recvbuf1 = (char*)malloc(SIZE_BUF*sizeof(char));
    char* recvbuf2 = (char*)malloc(SIZE_BUF*sizeof(char));
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status *status;
    
    double start, end;
    
    if (rank%2 == 0) {
        start = MPI_Wtime();
        MPI_Send(sendbuf1, strlen(sendbuf1), MPI_CHAR, rank+1, tag, MPI_COMM_WORLD);
        MPI_Recv(recvbuf1, SIZE_BUF * sizeof(char), MPI_CHAR, rank+1, tag, MPI_COMM_WORLD, status);
        end = MPI_Wtime();
        printf("Messege by rank%d : %s, time: %f\n",rank, recvbuf1, (end-start)/2);
    }
    else if (rank%2 == 1) {
        start = MPI_Wtime();
        MPI_Recv(recvbuf2, SIZE_BUF * sizeof(char), MPI_CHAR, rank-1, tag, MPI_COMM_WORLD, status);
        MPI_Send(sendbuf2, strlen(sendbuf2), MPI_CHAR, rank-1, tag,  MPI_COMM_WORLD);
        end = MPI_Wtime();
        printf("Messege by rank%d : %s, time: %f\n",rank, recvbuf2, (end - start)/2);
    }
    MPI_Finalize();
}
