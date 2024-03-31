#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "mpi.h"
#define SIZE_BUF 1024

int main(int argc, char *argv[]) {
    int procs, rank, namelen, count, source, tag;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    char* sendbuf ="Privet";
    char* recvbuf = malloc(SIZE_BUF*sizeof(char));
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Status status;
        
    if (rank == 0) {
	int count_recv = 1;
	while (count_recv < procs) {
	    MPI_Recv(recvbuf, SIZE_BUF*sizeof(char), MPI_CHAR, count_recv, tag, MPI_COMM_WORLD, &status);
            printf("Take blocked messege from rank %d: %s\n", count_recv, recvbuf);
            count_recv++;
        }
    }
    
    else {
        
        MPI_Send(sendbuf, strlen(sendbuf), MPI_CHAR, 0, tag, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}
