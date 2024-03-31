
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int mybcast(char* buffer, int datacount, int root)
{
	int size, rank;
	if ((MPI_Comm_rank(MPI_COMM_WORLD, &rank)) || (MPI_Comm_size(MPI_COMM_WORLD, &size)))
		return 1;

	MPI_Status status;
	MPI_Request request;


	for (int i = 0; i < size; i++)
	{
		if (i == rank)
			continue;

		if (rank == root)
			MPI_Isend(buffer, datacount, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
		else
			MPI_Irecv(buffer, datacount, MPI_CHAR, root, 1, MPI_COMM_WORLD, &request);
	}


	MPI_Barrier(MPI_COMM_WORLD);
	return 0;
}

int main(int argc, char* argv[])
{

	int opt;
	int root;
	int rank, size;
	int message_size;
	double start, stop, start_2, end_2;
	while ((opt = getopt(argc, argv, "r:t:")) != -1) {
		switch (opt) {
		case 'r':
			root = atoi(optarg);
			break;
		case 't':
			message_size = atoi(optarg);
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
		perror("Error at MPI_Comm_rank!\n");
	if (root >= size)
		fprintf(stderr, "Choosen rank is more than size of comm, try less rank.\n");
	char buffer[message_size];

	if (rank == root)
	{
		for (int i = 0; i < message_size; i++)
			buffer[i] = char(65);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	start = MPI_Wtime();
	mybcast(buffer, message_size, root);
	stop = MPI_Wtime();
	if (rank == root)
		printf("(root) rank=%d, time=%f, buffer=\"%s\"\n", rank,  stop - start, buffer);

	else
		printf("rank=%d, buffer=\"%s\"\n", rank, buffer);
	MPI_Barrier(MPI_COMM_WORLD);

	start_2 = MPI_Wtime();
	MPI_Bcast(buffer, message_size, MPI_CHAR, 0, MPI_COMM_WORLD);
	end_2 = MPI_Wtime();
	if (rank == root)
		printf("MPI_Bcast\nrank:%d\tbuffer=\"%s\"\ttime:%f\n", rank, buffer, end_2 - start_2);
	MPI_Finalize();
	return 0;
}
