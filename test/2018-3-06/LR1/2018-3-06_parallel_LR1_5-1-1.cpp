#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	MPI_Init(&argc, &argv);
	int rank, size, prev, next, buf;
	int message = 5;
	MPI_Status stats;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	prev = rank - 1;
	next = rank + 1;
	if (rank == 0)
		prev = size - 1;

	if (rank == size - 1)
		next = 0;


	if (rank == 0) {
		MPI_Send(&message, 1, MPI_INT, next, 5, MPI_COMM_WORLD);
		MPI_Recv(&buf, 1, MPI_INT, prev, 5, MPI_COMM_WORLD, &stats);
	}
	else {
		MPI_Recv(&buf, 1, MPI_INT, prev, 5, MPI_COMM_WORLD, &stats);
		MPI_Send(&buf, 1, MPI_INT, next, 5, MPI_COMM_WORLD);
	}

	printf("process %d got and send =  %d\n", rank, buf);
	MPI_Finalize();
}
