#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int send_tag = 0, recv_tag = 0;
    MPI_Status status;
    int sendbuf = rank;
    int recvbuf = 0;
    
    MPI_Sendrecv(&sendbuf, 1, MPI_INT, rank == size-1 ? 0 : rank+1, send_tag, 
                 &recvbuf, 1, MPI_INT, rank - 1, recv_tag, MPI_COMM_WORLD, &status);
    
    printf("Rank %d got message from rank %d\n", rank, rank == 0 ? size-1 : rank - 1);
    
    MPI_Finalize();
}