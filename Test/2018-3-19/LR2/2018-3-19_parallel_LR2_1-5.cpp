////1.	С помощью неблокируемых операций точечного взаимодействия реализовать каждую 
////из функций коллективного взаимодействия
////MPI_Allgather
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
	MPI_Request request[MAX_PROC];
	MPI_Status status[MAX_PROC];
	int rank_0_mass[MAX_PROC];
	for (int i = 0; i < n; i++)
	{
		rank_0_mass[i] = i;
	}

	start_1 = MPI_Wtime();
	for (int i = 0; i < n; i++)
	{
		MPI_Irecv(&recvbuf_1[i], 1, MPI_CHAR, i, 22, MPI_COMM_WORLD, &request[1]);
		MPI_Isend(&rank, 1, MPI_CHAR, i, 22, MPI_COMM_WORLD, &request[0]);
		MPI_Waitall(2, request, status);
	}
	end_1 = MPI_Wtime();
	printf("rank:%d\t", rank);
	for (int i = 0; i < n; i++)
	{
		printf("|%d|", rank_0_mass[i]);
	}
	printf("\n");
	if (rank == n - 1)
	{
		printf("Time MY_Allgether: %f\n", end_1 - start_1);
	}

	start_2 = MPI_Wtime();
	MPI_Allgather (&rank, 1, MPI_INT, recvbuf_2, 1, MPI_INT, MPI_COMM_WORLD );
	end_2 = MPI_Wtime();
	printf("rank:%d\t", rank);
	for (int i = 0; i < n; i++)
	{
		printf("|%d|", recvbuf_2[i]);
	}
	printf("\n");
	if (rank == n - 1)
	{
		printf("Time MPI_Allgather: %f\n", end_2 - start_2);
	}
	MPI_Finalize();
	return 0;
}
