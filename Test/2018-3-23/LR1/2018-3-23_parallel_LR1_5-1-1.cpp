#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int tag = 0;
    MPI_Status status;

    int max_count = 1;
    int sendbuf = rank;
    int recvbuf = 0;
    
    if (rank == 0)
    {
        MPI_Send(&sendbuf, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
        MPI_Recv(&recvbuf, 1, MPI_INT, size - 1, tag, MPI_COMM_WORLD, &status);
        printf("Rank %d got message from rank %d\n", rank, recvbuf);
    }
    else
    {
        MPI_Recv(&recvbuf, 1, MPI_INT, rank - 1, tag, MPI_COMM_WORLD, &status);
        printf("Rank %d got message from rank %d\n", rank, recvbuf);
        MPI_Send(&sendbuf, 1, MPI_INT, rank == size - 1 ? 0 : rank + 1, tag, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}