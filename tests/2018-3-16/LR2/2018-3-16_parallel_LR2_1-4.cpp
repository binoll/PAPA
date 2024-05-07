#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int myscatter(char** sendbuf, char* recvbuf, int datacount, int send_rank)
{
	int size, rank;
	if ((MPI_Comm_rank(MPI_COMM_WORLD, &rank)) || (MPI_Comm_size(MPI_COMM_WORLD, &size)))
		return 1;

	MPI_Status status;
	MPI_Request request;

	for (int i = 0; i < size; i++)
	{
		if (rank == send_rank)
			MPI_Isend(sendbuf[i], datacount, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
		if (rank == i)
			MPI_Irecv(recvbuf, datacount, MPI_CHAR, send_rank, 1, MPI_COMM_WORLD, &request);
	}

	MPI_Barrier(MPI_COMM_WORLD);
	return 0;
}

int main(int argc, char* argv[])
{
	int opt;
	int	send_rank;
	int rank, size;
	double start_1, end_1, start_2, end_2;
	while ((opt = getopt(argc, argv, "r:")) != -1) {
		switch (opt) {
		case 'r':
			send_rank = atoi(optarg);
			break;
		default:
			fprintf(stderr, "Use flag -r to choose root rank\n");
			exit(EXIT_FAILURE);
		}
	}
	MPI_Init(&argc, &argv);

	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_Comm_rank!\n");
	if (MPI_Comm_size(MPI_COMM_WORLD, &size))
		perror("Error at MPI_Comm_size!\n");


	char** sendbuf = new char* [size];
	for (int i = 0; i < size; i++)
		sendbuf[i] = new char[size];

	char* recvbuf = new char[size];

	if (rank == send_rank)
	{
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				sendbuf[i][j] = char(65 + size * i + j);
	}
	start_1 = MPI_Wtime();
	myscatter(sendbuf, recvbuf, size, send_rank);
	end_1 = MPI_Wtime();
	if (rank == send_rank)
	{
		printf("Time MY_Scatter: %f\n", end_1 - start_1);
		printf("rank:%d, send: \"%.*s\"\n", rank, size, sendbuf[0]);
		for (int i = 1; i < size; i++)
			printf("\tsend: \"%.*s\"\n", size, sendbuf[i]);
	}
	printf("rank:%d, recv: \"%.*s\"\n", rank, size, recvbuf);
	MPI_Barrier(MPI_COMM_WORLD);

	start_2 = MPI_Wtime();
	MPI_Scatter(sendbuf, size, MPI_CHAR, recvbuf, size, MPI_CHAR, send_rank, MPI_COMM_WORLD);
	end_2 = MPI_Wtime();
	if (rank == send_rank)
	{
		printf("Time MPI_Scatter: %f\n", end_2 - start_2);
	}
	MPI_Finalize();
	return 0;
}