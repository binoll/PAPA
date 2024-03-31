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

    int sendbuf = rank;
    int recvbuf = 0;
    
    if (rank == 0)
    {
        int i = 1;
        for (i; i < size; ++i)
        {
            MPI_Send(&sendbuf, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
            MPI_Recv(&recvbuf, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
            printf("Rank %d got message from rank %d\n", 0, recvbuf);
        }
    }
    else
    {
        MPI_Recv(&recvbuf, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        printf("Rank %d got message from rank %d\n", rank, recvbuf);
        MPI_Send(&sendbuf, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}