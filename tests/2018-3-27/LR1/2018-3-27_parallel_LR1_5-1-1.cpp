#include <stdio.h>
#include <unistd.h>
#include "mpi.h"

#define BUFFSIZE 1


int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	int size, rank;
	int tag = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int text_send = rank;
	int text_recv = 0;
 
	MPI_Status status;

	if (rank == 0)
	{ 
		printf("Rank %d send message to rank %d\n", rank, rank+1);
		MPI_Send(&text_send, BUFFSIZE, MPI_INT, 1, tag, MPI_COMM_WORLD);
		MPI_Recv(&text_recv, BUFFSIZE, MPI_INT, size-1, tag, MPI_COMM_WORLD, &status);
	}
	else
	{ 
		MPI_Recv(&text_recv, BUFFSIZE, MPI_INT, rank - 1, tag, MPI_COMM_WORLD, &status);
		if (rank == size-1)
		{ 
			MPI_Send(&text_send, BUFFSIZE, MPI_INT, 0, tag, MPI_COMM_WORLD);
			printf("Rank %d send message to rank %d\n", rank, 0);
		}
		else
		{
			MPI_Send(&text_send, BUFFSIZE, MPI_INT, rank+1, tag, MPI_COMM_WORLD);
			printf("Rank %d send message to rank %d\n", rank, rank+1);
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
}
