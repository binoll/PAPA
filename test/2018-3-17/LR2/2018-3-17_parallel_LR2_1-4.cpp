/*
	Реализация разброса данных
*/
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SEND_RANK 0
#define MESSAGE_SIZE 5

int custom_scatter(char** sendbuf, char* recvbuf, int datacount, int send_rank)
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
		if (rank == send_rank)
			MPI_Isend(sendbuf[i], datacount, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
		if (rank == i)
			MPI_Irecv(recvbuf, datacount, MPI_CHAR, send_rank, 1, MPI_COMM_WORLD, &request);
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
		perror("Error at MPI_Comm_size!\n");


	char** sendbuf = new char* [size];
	for (int i = 0; i < size; i++)
		sendbuf[i] = new char[MESSAGE_SIZE];

	char* recvbuf = new char[MESSAGE_SIZE];
	
	if (rank == SEND_RANK)
	{
		for (int i = 0; i < size; i++)
			for (int j = 0; j < MESSAGE_SIZE; j++)
				sendbuf[i][j] = char(int('a') + MESSAGE_SIZE * i + j);
	}

	custom_scatter(sendbuf, recvbuf, MESSAGE_SIZE, SEND_RANK);

	if (rank == SEND_RANK)
	{
		printf("rank=%d, send: \"%.*s\"\n", rank, MESSAGE_SIZE, sendbuf[0]);
		for (int i = 1; i < size; i++)
			printf("\tsend: \"%.*s\"\n", MESSAGE_SIZE, sendbuf[i]);
	}
	
	printf("rank=%d, recv: \"%.*s\"\n", rank, MESSAGE_SIZE, recvbuf);

	MPI_Finalize();
	return 0;
}
