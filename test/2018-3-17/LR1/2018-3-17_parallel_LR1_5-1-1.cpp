/*
5.1.1 Передача данных по кольцу: “эстафетная палочка” (очередной процесс дожидается 
сообщения от предыдущего и потом посылает следующему)
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
	char* buf = new char;
	char recv, send;

	int destination = (rank == size - 1) ? 0 : rank + 1;
	int source = (rank == 0) ? size - 1 : rank - 1;

	if (rank == 0)
	{
		*buf = 'G';

		if (MPI_Send(buf, 1, MPI_CHAR, destination, 1, MPI_COMM_WORLD))
			fprintf(stderr, "MPI_SEND ERROR\n");
		send = *buf;

		if (MPI_Recv(buf, 1, MPI_CHAR, source, 1, MPI_COMM_WORLD, &status))
			fprintf(stderr, "MPI_RECV ERROR!\n");
		recv = *buf;
	
	}
	else
	{
		if (MPI_Recv(buf, 1, MPI_CHAR, source, 1, MPI_COMM_WORLD, &status))
			fprintf(stderr, "MPI_RECV ERROR!\n");
		recv = *buf;

		if (MPI_Send(buf, 1, MPI_CHAR, destination, 1, MPI_COMM_WORLD))
			fprintf(stderr, "MPI_SEND ERROR\n");
		send = *buf;
	}
	
	printf("(src: %d, dest: %d) rank=%d, send='%c', recv='%c'\n", source, destination, rank, send, recv);

	delete buf;
	MPI_Finalize();
	return 0;
}
