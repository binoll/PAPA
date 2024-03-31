#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int myalltoall(int* sendbuf, int* recvbuf, int datacount, MPI_Datatype datatype)
{
	int size, rank;
	if ((MPI_Comm_rank(MPI_COMM_WORLD, &rank)) || (MPI_Comm_size(MPI_COMM_WORLD, &size)))
		return 1;

	MPI_Status status;

	for (int i = 0; i < size; i++) {
		MPI_Sendrecv(&sendbuf[i], 1, MPI_INT, i, 1,
			&recvbuf[i], 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
			MPI_COMM_WORLD, &status);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	return 0;
}

int main(int argc, char* argv[])
{
	double start_1, end_1, start_2, end_2;
	MPI_Init(&argc, &argv);

	int size, rank;
	if (MPI_Comm_size(MPI_COMM_WORLD, &size))
		perror("Error at MPI_Comm_size!\n");
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_Comm_rank!\n");

	int sendbuf[size], recvbuf[size];

	for (int i = 0; i < size; i++)
		sendbuf[i] = rank * size + i + 1;

	start_1 = MPI_Wtime();
	myalltoall(sendbuf, recvbuf, size, MPI_INT);
	end_1 = MPI_Wtime();
	if (rank == size-1)
	{
		printf("Time MY_Alltoall: %f\n", end_1 - start_1);
	}
	printf("rank:%d, send=[ ", rank);
	for (int i = 0; i < size; i++)
		printf("%d ", sendbuf[i]);
	printf("], ");

	printf("recv:[ ");
	for (int i = 0; i < size; i++)
		printf("%d ", recvbuf[i]);
	printf("]\n");
	MPI_Barrier(MPI_COMM_WORLD);


	start_2 = MPI_Wtime();
	MPI_Alltoall(sendbuf, size, MPI_INT, recvbuf, size, MPI_INT, MPI_COMM_WORLD);
	end_2 = MPI_Wtime();
	if (rank == size - 1)
	{
		printf("Time MPI_Alltoall: %f\n", end_2 - start_2);
	}

	MPI_Finalize();
	return 0;
}