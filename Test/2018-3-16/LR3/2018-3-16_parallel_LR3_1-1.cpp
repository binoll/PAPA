#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int sendbuf[size];
	int recvbuf[size];
	printf("rank:%d\tsendbuf: ", rank);
	for (int i = 0; i < size; i++)
	{
		sendbuf[i] = i+rank;
		printf("%d ", sendbuf[i]);
	}
	printf("\n");
	MPI_Reduce(sendbuf, recvbuf, size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		printf("rank:%d\trecvbuf: ", rank);
		for (int i = 0; i < size; i++)
		{
			printf("%d ", recvbuf[i]);
		}
		printf("\n");
	}
	MPI_Finalize();
	return 0;
}