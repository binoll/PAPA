#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

int main(int argc, char* argv[])
{
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Status a;
	MPI_Request req;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//Транспонирование
	int matrix[size], matrix_recv[size];
	for (int i = 0; i < size; i++) {
		matrix[i] = rank;
	}
	for (int i = 0; i < size; i++) {
		printf("%d ", matrix[i]);
	}
	printf("\n");
	for (int i = 0; i < size; i++)
	{
		MPI_Isend(&matrix[i], 1, MPI_INT, i, 1, MPI_COMM_WORLD, &req);
	}

	for (int j = 0; j < size; j++)
	{
		MPI_Irecv(&matrix_recv[j], 1, MPI_INT, j, 1, MPI_COMM_WORLD, &req);
		MPI_Wait(&req, &a);
	}

	for (int i = 0; i < size; i++) {
		printf("%d ", matrix_recv[i]);
	}
	
	printf("    rank - %d   \n", rank);
	MPI_Finalize();

	return 0;
}


