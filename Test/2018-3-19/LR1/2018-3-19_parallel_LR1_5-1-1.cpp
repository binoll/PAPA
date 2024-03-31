#include <stdio.h>
#include <string.h>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int n, rank, rank_from, rank_to;
    char recvbuf[MAX_BUF_SIZE];
    char sendbuf[MAX_BUF_SIZE];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Status status;
    rank_from = (n + rank - 1) % n;
    rank_to = (rank + 1) % n;
    sprintf(sendbuf, "Hello from rank %d", rank);
    if (rank == 0)
    {
	MPI_Send(sendbuf, strlen(sendbuf), MPI_CHAR, rank_to, 22, MPI_COMM_WORLD);
	MPI_Recv(recvbuf, MAX_BUF_SIZE, MPI_CHAR, rank_from, 22, MPI_COMM_WORLD, &status);
    }
    else
    {
	MPI_Recv(recvbuf, MAX_BUF_SIZE, MPI_CHAR, rank_from, 22, MPI_COMM_WORLD, &status);
	MPI_Send(sendbuf, strlen(sendbuf), MPI_CHAR, rank_to, 22, MPI_COMM_WORLD);
    }
    printf("From rank: %d\tOn rank: %d\tTo rank: %d\tMessage: %s\n", rank_from, rank, rank_to, recvbuf);
    MPI_Finalize();
    return 0;
}