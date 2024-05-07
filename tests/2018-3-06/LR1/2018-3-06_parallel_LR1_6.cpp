#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int size, rank, tag = 1;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int sendbuf[size], recvbuf[size];
	MPI_Status status[size - 1];
	MPI_Request request[size - 1];

	for (int i = 0; i < size; i++)
		sendbuf[i] = rank * size + i;

	for (int i = 0, req_idx = 0; i < size; i++)
	{
		if (i == rank)
			continue;

		MPI_Isend(&sendbuf[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD, &request[req_idx]);
		req_idx++;
	}

	for (int i = 0, st_idx = 0, j = 0; i < size; i++)
	{
		if (i == rank)
			recvbuf[i] = sendbuf[i];
		else
		{
			MPI_Recv(&recvbuf[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status[st_idx]);
			st_idx++;
		}
	}

	MPI_Waitall(size - 1, request, status);

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
