/*
5.1.2 Передача данных по кольцу: “сдвиг”(одновременная посылка и прием сообщений)
*/
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int size, rank;
	if (MPI_Comm_size(MPI_COMM_WORLD, &size))
		perror("Error at MPI_Comm_size!\n");
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_Comm_rank!\n");

	MPI_Status status;

	char* recv = new char;
	char* send = new char;

	int destination = (rank == size - 1) ? 0 : rank + 1;
	int source = (rank == 0) ? size - 1 : rank - 1;

	*send = char(int('A') + rank);

	MPI_Sendrecv(send, 1, MPI_CHAR, destination, 1, 
				 recv, 1, MPI_CHAR, source, 1,
			     MPI_COMM_WORLD, &status);
		
	printf("rank=%d, recv from %d: '%c', send to %d: '%c'\n", rank, source, *recv, destination, *send);

	delete send, recv;
	MPI_Finalize();
	return 0;
}
