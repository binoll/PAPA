#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv)
{
	int i, nproc, rank, tag = 99, j = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Request request[2 * (nproc - 1)];
	MPI_Status status[2 * (nproc - 1)];
	int buf[nproc - 1];
	for (i = 0; i < nproc; i++)
		if (rank != i)
		{
			MPI_Isend(&rank, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &request[j]);
			MPI_Irecv(&buf[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD, &request[nproc - 1 + j]);
			j += 1;
		}
	MPI_Waitall(2 * (nproc - 1), request, status);
	printf("process %d got messages: ", rank);
	for (i = 0; i < nproc; i++)
		if (rank != i)
			printf("%d ", buf[i]);
	printf(" \n");
	MPI_Finalize();
}
