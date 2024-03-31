/*
	����������������� ��������
*/
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SEND_RANK 0

int custom_bcast(char* buffer, int datacount, int root)
{
	int size, rank;
	if (MPI_Comm_size(MPI_COMM_WORLD, &size))
		return 1;
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		return 1;

	MPI_Status status;
	MPI_Request request;


	for (int i = 0; i < size; i++)
	{
		if (i == rank)
			continue;

		if (rank == root)
			MPI_Isend(buffer, datacount, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
		else
			MPI_Irecv(buffer, datacount, MPI_CHAR, root, 1, MPI_COMM_WORLD, &request);
	}


	MPI_Barrier(MPI_COMM_WORLD);
	return 0;
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int rank, size;
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_Comm_rank!\n");
	if (MPI_Comm_size(MPI_COMM_WORLD, &size))
		perror("Error at MPI_Comm_rank!\n");

	char buffer[size];

	if (rank == SEND_RANK)
	{
		for (int i = 0; i < size; i++)
			buffer[i] = char(int('a') + i);
	}

	custom_bcast(buffer, size, SEND_RANK);
	
	if (rank == SEND_RANK)
		printf("(root) rank=%d, buffer=\"%s\"\n", rank, buffer);
	else
		printf("rank=%d, buffer=\"%s\"\n", rank, buffer);

	MPI_Finalize();
	return 0;
}
