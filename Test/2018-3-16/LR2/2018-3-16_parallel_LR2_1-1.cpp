#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int mybarrier(MPI_Comm comm)
{
	int rank, size;

	if ((MPI_Comm_rank(MPI_COMM_WORLD, &rank)) || (MPI_Comm_size(MPI_COMM_WORLD, &size)))
		return 1;

	char* sendbuf = new char;
	char* recvbuf = new char;

	MPI_Request request[size];
	MPI_Status status[size];

	for (int i = 0; i < size; i++)
	{
		MPI_Isend(sendbuf, 1, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request[i]);
		MPI_Irecv(recvbuf, 1, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request[i]);
	}

	MPI_Waitall(size, request, status);

	return 0;
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int rank, size;
	double start, stop;

	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_Comm_rank!\n");
	if (MPI_Comm_size(MPI_COMM_WORLD, &size))
		perror("Error at MPI_Comm_size!\n");

	start = MPI_Wtime();
	if (rank == 1)
	{
		printf("Time with barrier:\n");
		sleep(5);
	}
	mybarrier(MPI_COMM_WORLD);
	stop = MPI_Wtime();
	printf("rank=%d, time=%f\n", rank, stop - start);

	MPI_Finalize();
	return 0;
}