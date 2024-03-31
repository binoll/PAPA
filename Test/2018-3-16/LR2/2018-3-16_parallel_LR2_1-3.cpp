#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>



int mygather(char* sendbuf, char** recvbuf, int datacount, int recv_rank)
{
	int size, rank;
	if ((MPI_Comm_rank(MPI_COMM_WORLD, &rank)) || (MPI_Comm_size(MPI_COMM_WORLD, &size)))
		return 1;

	MPI_Status status;
	MPI_Request request;


	MPI_Isend(sendbuf, datacount, MPI_CHAR, recv_rank, 1, MPI_COMM_WORLD, &request);

	if (rank == recv_rank)
	{
		for (int i = 0; i < size; i++)
		{
			MPI_Irecv(recvbuf[i], datacount, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
		}
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
			fprintf(stderr, "Use flag -r to choose root rank, -t to set up size of message \n");
			exit(EXIT_FAILURE);
		}
	}
	MPI_Init(&argc, &argv);
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_Comm_rank!\n");
	if (MPI_Comm_size(MPI_COMM_WORLD, &size))
		perror("Error at MPI_Comm_size!\n");

	char** recvbuf = new char* [size];
	for (int i = 0; i < size; i++)
		recvbuf[i] = new char[size];

	char* sendbuf = new char[size];
	for (int i = 0; i < size; i++)
		sendbuf[i] = char(65 + size * rank + i);
	start_1 = MPI_Wtime();
	mygather(sendbuf, recvbuf, size, send_rank);
	end_1 = MPI_Wtime();
	if (rank == send_rank)
	{
		printf("Time MY_Gather: %f\n", end_1 - start_1);
		for (int i = 0; i < size; i++)
			printf("recv rank=%d, recv=\"%s\"\n", rank, recvbuf[i]);
	}
	else
		printf("rank=%d, send: \"%s\"\n", rank, sendbuf);
	MPI_Barrier(MPI_COMM_WORLD);

	start_2 = MPI_Wtime();
	MPI_Gather(sendbuf, size, MPI_CHAR, recvbuf, size, MPI_CHAR, send_rank, MPI_COMM_WORLD);
	end_2 = MPI_Wtime();
	if (rank == send_rank)
	{
		printf("Time MPI_Gather: %f\n", end_2 - start_2);
		
	}
	MPI_Finalize();
	return 0;
}