/*
5.3 ѕересылка данных от каждого процесса каждому
*/
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define MASTER_RANK 0

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int n, rank;
	if (MPI_Comm_size(MPI_COMM_WORLD, &n))
		perror("Error at MPI_Comm_size!\n");
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_Comm_rank!\n");

	MPI_Status status;
	MPI_Request request;

	char* recv = new char;
	char* send = new char;
	*send = char(int('a') + rank);

	for (int i = 0; i < n; i++) 
	{
		if (i == rank)
			continue;
	
		if (MPI_Isend(send, 1, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request))
			fprintf(stderr, "MPI_SEND ERROR\n");

		if (MPI_Recv(recv, 1, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status))
			fprintf(stderr, "MPI_RECV ERROR!\n");
		
		printf("rank=%d from rank=%d, recv='%c'\n", rank, i, *recv);
	}
	MPI_Finalize();
	return 0;
}
