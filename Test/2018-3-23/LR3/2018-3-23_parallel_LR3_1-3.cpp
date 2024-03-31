#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
    int opt, size, n, rank;
    while ((opt = getopt(argc, argv, "s:")) != -1)
    {
        switch (opt)
        {
        case 's':
            size = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Bad argument");
            exit(EXIT_FAILURE);
        }
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n);

    int sendbuf[size+1];
    for (int i = 0; i < size + 1; ++i)
    {
      sendbuf[i] = rank + i;
    }
    printf("rank %d, send: ", rank);
    
    for(int i = 0; i < size; ++i)
    {
      printf("%d ", sendbuf[i]);
    }
    printf("\n");

    int count[n]; 
    for (int i = 0; i < n; ++i)
    {
      count[i] = 1;
    }
    count[n - 1] = 2;

    if (rank == n - 1)
    {
       int recvbuf[2];
       MPI_Reduce_scatter(sendbuf, &recvbuf, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
       printf("Rank:%d, recvbuf:%d %d\n", rank, recvbuf[0], recvbuf[1]);
    }
    else
    {
       int recvbuf;
       MPI_Reduce_scatter(sendbuf, &recvbuf, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
       printf("Rank:%d, recvbuf:%d\n", rank, recvbuf);
    }

    MPI_Finalize();
	return 0;
}