////1.	С помощью неблокируемых операций точечного взаимодействия реализовать каждую 
////из функций коллективного взаимодействия
////MPI_Scatter
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
	int recvbuf_1;
	int recvbuf_2;
	MPI_Request request[MAX_PROC];
	MPI_Status status[MAX_PROC];
	int rank_0_mass_1[MAX_PROC];
	int rank_0_mass_2[MAX_PROC];

	start_1 = MPI_Wtime();
	if (rank == 0)
	{
		for (int i = 1; i < MAX_PROC; i++)
		{
			rank_0_mass_1[i-1] = i;
			MPI_Isend(&rank_0_mass_1[i - 1], 1, MPI_INT, i, 22, MPI_COMM_WORLD, &request[i - 1]);
		}
		MPI_Waitall(n - 1, request, status);
	}
	else
	{
		MPI_Irecv(&recvbuf_1, 1, MPI_INT, 0, 22, MPI_COMM_WORLD, &request[rank - 1]);
		MPI_Wait(&request[rank - 1], &status[rank - 1]);
		printf("rank:%d\tmessage from rank 0:%d\n", rank, recvbuf_1);
	}
	end_1 = MPI_Wtime();

	if (rank == n - 1)
	{
		printf("Time MY_Scatter: %f\n", end_1 - start_1);
	}
	
	for (int i =0; i < n; i++)
	{
		rank_0_mass_2[i] = i;
	}
	start_2 = MPI_Wtime();
	MPI_Scatter (rank_0_mass_2, 1, MPI_INT, &recvbuf_2, 1, MPI_INT, 0, MPI_COMM_WORLD);
	end_2 = MPI_Wtime();
	printf("rank:%d\tmessage:%d\n", rank, recvbuf_2);
	if (rank == n - 1)
	{
		printf("Time MPI_Scatter: %f\n", end_2 - start_2);
	}

	MPI_Finalize();
	return 0;
}
