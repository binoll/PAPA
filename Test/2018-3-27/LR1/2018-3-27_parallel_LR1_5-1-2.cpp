#include "mpi.h"
#include <stdio.h>

#define STR_LEN 4


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
 
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;

	int tag = 0;
	int sendbuf = rank;
	int recvbuf;

	MPI_Sendrecv(&sendbuf, STR_LEN, MPI_INT, rank == size-1 ? 0 : rank+1, tag, &recvbuf, STR_LEN, MPI_INT, rank - 1, tag, MPI_COMM_WORLD, &status);
 
	printf("Rank: %d. Message from rank: %d\n", rank, rank == 0 ? size: rank - 1);
 
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
}
