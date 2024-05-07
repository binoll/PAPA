#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int pred, next, buf, nproc, rank, message = 5, tag = 99;
	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Request request;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	pred = rank - 1;
	next = rank + 1;
	if (rank == 0)
		pred = nproc - 1;

	if (rank == nproc - 1)
		next = 0;
	if (rank == 0) {
		MPI_Send(&message, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
		MPI_Recv(&buf, 1, MPI_INT, pred, tag, MPI_COMM_WORLD, &status);
	}
	else {
		MPI_Recv(&buf, 1, MPI_INT, pred, tag, MPI_COMM_WORLD, &status);
		MPI_Send(&buf, 1, MPI_INT, next, tag, MPI_COMM_WORLD);
	}
	printf("process %d got and send =  %d\n", rank, buf);

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Sendrecv(&rank, 1, MPI_INT, next, tag, &buf, 1, MPI_INT, pred, tag, MPI_COMM_WORLD, &status);
	printf("process with rank %d in sendrecv got %d and send %d\n", rank, buf, next);
	MPI_Finalize();
}
