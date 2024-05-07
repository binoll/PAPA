/*
	���������� ����� ������
*/
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SEND_RANK 0
#define MESSAGE_SIZE 5

int custom_scatter(char* sendbuf, char** recvbuf, int datacount, int recv_rank)
{
	int size, rank;
	if (MPI_Comm_size(MPI_COMM_WORLD, &size))
		return 1;
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		return 1;

	MPI_Status status;
	MPI_Request request;


	MPI_Isend(sendbuf, datacount, MPI_CHAR, recv_rank, 1, MPI_COMM_WORLD, &request);
	
	if (rank == recv_rank)
	{
		for (int i = 0; i < size; i++) 
		{
			MPI_Irecv(recvbuf[i], datacount, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
		}
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
		recvbuf[i] = new char[size];
	
	char* sendbuf = new char[size];
	for (int i = 0; i < size; i++)
		sendbuf[i] = char(int('a') + rank * size + i);

	custom_scatter(sendbuf, recvbuf, size, SEND_RANK);
	
	if (rank == SEND_RANK)
	{
		for (int i = 0; i<size; i++)
			printf("recv rank=%d, recv=\"%s\"\n", rank, recvbuf[i]);
	}
	else
		printf("rank=%d, send: \"%s\"\n", rank, sendbuf);

	MPI_Finalize();
	return 0;
}
