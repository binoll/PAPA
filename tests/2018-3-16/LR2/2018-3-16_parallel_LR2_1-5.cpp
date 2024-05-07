#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int myallgather(char* sendbuf, char** recvbuf, int datacount)
{
	int size, rank;
	if ((MPI_Comm_rank(MPI_COMM_WORLD, &rank)) || (MPI_Comm_size(MPI_COMM_WORLD, &size)))
		return 1;

	MPI_Status status;
	MPI_Request request;

	for (int i = 0; i < size; i++)
	{
		MPI_Irecv(recvbuf[i], datacount, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
		MPI_Isend(sendbuf, datacount, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	return 0;
}

int main(int argc, char* argv[])
{

	int rank, size;
	double start_1, end_1, start_2, end_2;
	MPI_Init(&argc, &argv);

	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_Comm_rank!\n");
	if (MPI_Comm_size(MPI_COMM_WORLD, &size))
		perror("Error at MPI_Comm_size!\n");

	char** recvbuf = new char* [size];
	for (int i = 0; i < size; i++)
		recvbuf[i] = new char[size];

	char* sendbuf = new char[size];
	for (int i = 0; i < size; i++)
		sendbuf[i] = char(65 + rank * size + i);
	start_1 = MPI_Wtime();
	myallgather(sendbuf, recvbuf, size);
	end_1 = MPI_Wtime();
	if (rank == size - 1)
	{
		printf("Time MY_Allgether: %f\n", end_1 - start_1);
	}
	printf("rank:%d, send: \"%.*s\"\n", rank, size, sendbuf);
	for (int i = 0; i < size; i++)
		printf("\trecv: \"%.*s\"\n", size, recvbuf[i]);
	MPI_Barrier(MPI_COMM_WORLD);

	start_2 = MPI_Wtime();
	MPI_Allgather(sendbuf, size, MPI_CHAR, recvbuf, size, MPI_CHAR, MPI_COMM_WORLD);
	end_2 = MPI_Wtime();
	if (rank == size-1)
	{
		printf("Time MPI_Allgather: %f\n", end_2 - start_2);
	}
	MPI_Finalize();
	return 0;
}