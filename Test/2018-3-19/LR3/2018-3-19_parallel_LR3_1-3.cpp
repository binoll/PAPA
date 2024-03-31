/*
Разработать параллельную программу, демонстрирующую применение
глобальных операций в MPI
MPI_Reduce_scatter
*/

#include <stdio.h>
#include "mpi.h"
#define MAX_PROC 8

int main(int argc, char* argv[])
{
	int n, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int sendbuf[MAX_PROC];
	int recvbuf[MAX_PROC];
	printf("rank:%d\tsend: ", rank);
	for (int i = 0; i < n ; i++)
	{
		sendbuf[i] = i;
		printf("|%d| ", sendbuf[i]);
	}
	printf("\n");

	int count[MAX_PROC];
	for (int i = 0; i < n; i++)
	{
		count[i] = 1;
	}

	MPI_Reduce_scatter(sendbuf, recvbuf, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	printf("rank:%d\trecv: ", rank);
	printf("|%d| ", recvbuf[0]);
	printf("\n");

	MPI_Finalize();
	return 0;
}