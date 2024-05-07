#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int n, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int send_tag = 0, recv_tag = 0;
    MPI_Status status;

    int max_count = 4;
    int sendbuf = rank;
    int recvbuf = -1;
    
    MPI_Sendrecv(&sendbuf, max_count, MPI_INT, rank == n-1 ? 0 : rank+1, send_tag, 
                 &recvbuf, max_count, MPI_INT, rank - 1, recv_tag, MPI_COMM_WORLD, &status);
    
    printf("Rank %d got message from rank %d\n", rank, rank == 0 ? n-1 : rank - 1);
    
    MPI_Finalize();
}