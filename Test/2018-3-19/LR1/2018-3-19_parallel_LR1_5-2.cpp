#include <stdio.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF_SIZE 1024
int main(int argc, char *argv[])
{
    int n, rank;
    char *recvbuf = malloc(MAX_BUF_SIZE);
    char sendbuf[MAX_BUF_SIZE];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Status status;
    if (rank == 0)
    {
	sprintf(sendbuf, "Hello from rank 0 (MASTER)");
	int i;
	for (i = 1; i < n; i++)
	{
	    MPI_Send(sendbuf, strlen(sendbuf), MPI_CHAR, i, 22, MPI_COMM_WORLD);
	    MPI_Recv(recvbuf, MAX_BUF_SIZE, MPI_CHAR, i, 22, MPI_COMM_WORLD, &status);
	    printf("message on rank 0: %s\n", recvbuf);
	}
    }
    else
    {
	MPI_Recv(recvbuf, MAX_BUF_SIZE, MPI_CHAR, 0, 22, MPI_COMM_WORLD, &status);
	printf("message on rank %d: %s\n", rank, recvbuf);
	sprintf(sendbuf, "Hello from rank %d (SLAVE)", rank);
	MPI_Send(sendbuf, strlen(sendbuf), MPI_CHAR, 0, 22, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}