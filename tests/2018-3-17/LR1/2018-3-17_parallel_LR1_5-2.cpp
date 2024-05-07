/*
5.2 master-slave (все процессы общаются с одним)
*/
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define MASTER_RANK 0

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int size, rank;
	if (MPI_Comm_size(MPI_COMM_WORLD, &size))
		perror("Error at MPI_Comm_size!\n");
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_Comm_rank!\n");

	MPI_Status status;
	
	if (rank == MASTER_RANK)
	{
		char* buf = new char;
		for (int i = 0; i < size; i++) 
		{
			if (i == MASTER_RANK) 
				continue;
			
			if (MPI_Recv(buf, 1, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status))
				fprintf(stderr, "MPI_RECV ERROR!\n");
			
			printf("Master\t(rank=%d), recv from rank=%d: '%c'\n", rank, i, *buf);
		}
	}
	else
	{
		char send = char(int('a') + rank - 1);

		if (MPI_Send(&send, 1, MPI_CHAR, MASTER_RANK, 1, MPI_COMM_WORLD))
			fprintf(stderr, "MPI_SEND ERROR\n");
	}

	MPI_Finalize();
	return 0;
}
