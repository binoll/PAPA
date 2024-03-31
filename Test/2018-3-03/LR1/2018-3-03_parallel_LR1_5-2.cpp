#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int n, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int tag = 0;
    MPI_Status status;

    int max_count = 1;
    int sendbuf = rank;
    int recvbuf = -1;
    
    if (rank == 0)
    {
        int i = 1;
        for (i; i < n; ++i)
        {
            MPI_Send(&sendbuf, max_count, MPI_INT, i, tag, MPI_COMM_WORLD);
            MPI_Recv(&recvbuf, max_count, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
            printf("Rank %d got message from rank %d\n", 0, recvbuf);
        }
    }
    else
    {
        MPI_Recv(&recvbuf, max_count, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        printf("Rank %d got message from rank %d\n", rank, recvbuf);
        MPI_Send(&sendbuf, max_count, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}