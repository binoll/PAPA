/*
	���������� ����� ������
*/
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define MESSAGE_SIZE 5

int custom_allgather(char* sendbuf, char** recvbuf, int datacount)
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
		MPI_Irecv(recvbuf[i], datacount, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
		MPI_Isend(sendbuf, datacount, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
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

	char** recvbuf = new char* [size];
	for (int i = 0; i < size; i++)
		recvbuf[i] = new char[MESSAGE_SIZE];

	char* sendbuf = new char[MESSAGE_SIZE];
	for (int i = 0; i < MESSAGE_SIZE; i++)
		sendbuf[i] = char(int('a') + rank * size + i);

	custom_allgather(sendbuf, recvbuf, MESSAGE_SIZE);

	printf("rank=%d, send: \"%.*s\"\n", rank, MESSAGE_SIZE, sendbuf);
	for(int i = 0; i < size; i++)
		printf("\trecv: \"%.*s\"\n", MESSAGE_SIZE, recvbuf[i]);

	MPI_Finalize();
	return 0;
}
