////1.	С помощью неблокируемых операций точечного взаимодействия реализовать каждую 
////из функций коллективного взаимодействия
////MPI_Gather
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
	int recvbuf = 0;
	MPI_Request request[MAX_PROC];
	MPI_Status status[MAX_PROC];
	int rank_0_mass_1[MAX_PROC];
	int rank_0_mass_2[MAX_PROC];

	start_1 = MPI_Wtime();
	if (rank == 0)
	{
		rank_0_mass_1[rank] = rank;
		for (int i = 1; i < n; i++)
		{
			MPI_Irecv(&recvbuf, 1, MPI_INT, i, 22, MPI_COMM_WORLD, &request[i - 1]);
			MPI_Wait(&request[i - 1], &status[i - 1]);
			rank_0_mass_1[i] = recvbuf;
			printf("rank: %d\tmessage from %d: %d\n", rank, i, recvbuf);
		}
	}

	else
	{
		MPI_Isend(&rank, 1, MPI_INT, 0, 22, MPI_COMM_WORLD, &request[rank - 1]);
		MPI_Wait(&request[rank - 1], &status[rank - 1]);
	}
	end_1 = MPI_Wtime();
	if (rank == 0)
	{
		printf("Time MY_Gather: %f\n", end_1 - start_1);
		printf("array:\n");
		for (int i = 0; i < MAX_PROC; i++)
		{
			printf("%d\n", rank_0_mass_1[i]);
		}
	}

	start_2 = MPI_Wtime();
	MPI_Gather(&rank, 1, MPI_INT, rank_0_mass_2, 1, MPI_INT, 0, MPI_COMM_WORLD);
	end_2 = MPI_Wtime();
	if (rank == 0)
	{
		printf("Time MPI_Gather: %f\n", end_2 - start_2);
		printf("array:\n");
		for (int i = 0; i < MAX_PROC; i++)
		{
			printf("%d\n", rank_0_mass_2[i]);
		}
	}
	MPI_Finalize();
	return 0;
}
