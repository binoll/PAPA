/*
���������� MPI_Alltoall
*/
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int custom_alltoall(int* sendbuf, int* recvbuf, int datacount, MPI_Datatype datatype)
{
	int size, rank;
	if (MPI_Comm_size(MPI_COMM_WORLD, &size))
		return 1;
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		return 1;

	MPI_Status status[size - 1];
	MPI_Request request[size - 1];

	for (int i = 0, req_idx = 0; i < size; i++)
	{
		if (i == rank)
			continue;

		MPI_Isend(&sendbuf[i], 1, MPI_INT, i, 1, MPI_COMM_WORLD, &request[req_idx]);
		req_idx++;
	}
	
	for (int i = 0, req_idx = 0; i < size; i++)
	{
		if (i == rank)
			recvbuf[i] = sendbuf[i];
		else
		{
			MPI_Irecv(&recvbuf[i], 1, MPI_INT, i, 1, MPI_COMM_WORLD, &request[req_idx]);
			req_idx++;
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	return 0;
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int size, rank;
	if (MPI_Comm_size(MPI_COMM_WORLD, &size))
		perror("Error at MPI_Comm_size!\n");
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_Comm_rank!\n");

	int sendbuf[size], recvbuf[size];

	for (int i = 0; i < size; i++)
		sendbuf[i] = rank * size + i + 1;

	
	custom_alltoall(sendbuf, recvbuf, size, MPI_INT);

	
	printf("rank=%d, send=[ ", rank);
	for (int i = 0; i < size; i++)
		printf("%d ", sendbuf[i]);
	printf("], ");

	printf("recv=[ ");
	for (int i = 0; i < size; i++)
		printf("%d ", recvbuf[i]);
	printf("]\n");

	MPI_Finalize();
	return 0;
}
