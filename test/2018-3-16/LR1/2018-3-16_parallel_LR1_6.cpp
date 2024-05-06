#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
    int rank, n;
    int i, k;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Status status[2 * n];
    MPI_Request requests[2 * n];

    int* sendbuf = (int*)malloc(n * sizeof(int));
    int* recvbuf = (int*)malloc(n * sizeof(int));

    for (k = 0; k < n; k++) 
    {
        sendbuf[k] = (k + n * rank);
    } 
    printf("rank: %d - send ", rank);
    for (i = 0; i < n; i++)
    {
        printf("%d ", sendbuf[i]);
    }
    printf("\n");

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0){}
    for (i = 0; i < n; i++) 
    {
        MPI_Isend(&sendbuf[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &requests[i]);
        MPI_Irecv(&recvbuf[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &requests[n + i]);
    }

    MPI_Waitall(2 * n, requests, status);
    printf("rank: %d - recv ", rank);
    for (i = 0; i < n; i++) 
    {
        printf("%d ", recvbuf[i]);
    }
    printf("\n");
    MPI_Finalize();


}