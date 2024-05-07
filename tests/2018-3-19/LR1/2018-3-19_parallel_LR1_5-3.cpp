#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <stdlib.h>

#define MAX_BUF_SIZE 4096

int main(int argc, char *argv[])
{
    int n, rank;
    char *recvbuf = malloc(MAX_BUF_SIZE);
    char sendbuf[MAX_BUF_SIZE];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Status status;
    sprintf(sendbuf, "Hello from rank %d", rank);
    int i;
    for (i = 0; i < n; i++)
    {
	MPI_Sendrecv(sendbuf, strlen(sendbuf), MPI_CHAR, i, 22,
		    recvbuf, MAX_BUF_SIZE, MPI_CHAR, MPI_ANY_SOURCE, 22,
		    MPI_COMM_WORLD, &status);
	printf("Message on rank %d: %s\n", rank, recvbuf);
    }
    MPI_Finalize();
    return 0;
}