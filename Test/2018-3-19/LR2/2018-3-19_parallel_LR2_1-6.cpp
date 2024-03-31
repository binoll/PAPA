////1.	С помощью неблокируемых операций точечного взаимодействия реализовать каждую 
////из функций коллективного взаимодействия
////MPI_Alltoall
#include <stdio.h>
#include "mpi.h"
#define MAX_PROC 8
int main(int argc, char* argv[])
{
	int n, rank;
	double start_1, end_1, start_2, end_2;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int recvbuf_1[MAX_PROC];
	int recvbuf_2[MAX_PROC];
	MPI_Request request[MAX_PROC * 2];
	MPI_Status status[MAX_PROC * 2];
	int sendbuf[MAX_PROC];
	printf("rank:%d\tsend\t", rank);
	for (int i = 0; i < n; i++)
	{
		sendbuf[i] = i + n * rank;
		printf("|%2d|", sendbuf[i]);
	}
	printf("\n");
	start_1 = MPI_Wtime();
	for (int i = 0; i < n; i++)
	{
		MPI_Isend(&sendbuf[i], 1, MPI_INT, i, 22, MPI_COMM_WORLD, &request[i]);
		MPI_Irecv(&recvbuf_1[i], 1, MPI_INT, i, 22, MPI_COMM_WORLD, &request[i + n]);
	}
	MPI_Waitall(2 * n, request, status);
	end_1 = MPI_Wtime();
	printf("rank:%2d\trecv MY_Alltoall\t", rank);
	for (int i = 0; i < n; i++)
	{
		printf("|%2d|", recvbuf_1[i]);
	}
	printf("\n");
	if (rank == n - 1)
	{
		printf("Time MY_Alltoall: %f\n", end_1 - start_1);
	}

	start_2 = MPI_Wtime();
	MPI_Alltoall(sendbuf, 1, MPI_INT, recvbuf_2, 1, MPI_INT, MPI_COMM_WORLD);
	end_2 = MPI_Wtime();
	printf("rank:%d\trecv MPI_Alltoall\t", rank);
	for (int i = 0; i < n; i++)
	{
		printf("|%2d|", recvbuf_2[i]);
	}
	printf("\n");
	if (rank == n - 1)
	{
		printf("Time MPI_Alltoall: %f\n", end_2 - start_2);
	}
	MPI_Finalize();
	return 0;
}
