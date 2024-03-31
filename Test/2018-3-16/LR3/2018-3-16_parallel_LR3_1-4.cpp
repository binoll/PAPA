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
	printf("rank:%d\tsend: ", rank);
	for (int i = 0; i < size; i++)
	{
		sendbuf[i] = i+rank;
		printf("%d ", sendbuf[i]);
	}
	printf("\n");
	MPI_Scan(sendbuf, recvbuf, size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	printf("rank:%d\trecv: ", rank);
	for (int i = 0; i < size; i++)
	{
		printf("%d ", recvbuf[i]);
	}
	printf("\n");

	MPI_Finalize();
	return 0;
}