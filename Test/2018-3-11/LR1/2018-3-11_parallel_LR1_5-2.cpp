#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv)
{
	int i, buf, nproc, rank, tag = 99;
	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Request request;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	if (rank == 0)
	{
		for (i = 1; i < nproc; i++) {
			MPI_Recv(&buf, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
			printf("got message from %d process\n", buf);
		}
	}
	else
	{
		MPI_Send(&rank, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
	}
	MPI_Finalize();
}
