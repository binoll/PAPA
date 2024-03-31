#include <stdio.h>
#include <unistd.h>
#include "mpi.h"

#define BUFFSIZE 1


int main(int argc, char* argv [])
{
	int rank, size;
	int tag = 0;

	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	MPI_Status status;

	if (rank == 0)
	{ 
		int send = 1;
		int recv = 0;
		int p;
		for(p = 1; p < size; p++)
		{
			printf("Master : Process P%d started reading\n",p);
			MPI_Send(&send, BUFFSIZE, MPI_INT, p, tag, MPI_COMM_WORLD);
			MPI_Recv(&recv, BUFFSIZE, MPI_INT, p, tag, MPI_COMM_WORLD, &status);
		}
	}
	else
	{ 
		int state;
		MPI_Recv(&state, BUFFSIZE, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
		MPI_Send(&state, BUFFSIZE, MPI_INT, 0, tag, MPI_COMM_WORLD);
		printf("Slave : P%d finished reading\n", rank);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
