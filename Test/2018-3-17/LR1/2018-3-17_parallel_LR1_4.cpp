/*
4. Сравнить время реализации пересылок данных между двумя выделенными
процессорами с блокировкой и без блокировки.
*/
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	int MAX_MESSAGE_SIZE = 5;
	int opt;
	while ((opt = getopt(argc, argv, "c:")) != -1) {
		switch (opt) {
		case 'c':
			MAX_MESSAGE_SIZE = atoi(optarg);
			break;
		case ' ':
			break;
		default:
			fprintf(stderr, ": %s [-n process_id]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	int* buf_send = new int[MAX_MESSAGE_SIZE];
	for (int i = 0; i < MAX_MESSAGE_SIZE; i++)
		buf_send[i] = i;

	int *buf_recv = new int [MAX_MESSAGE_SIZE];

	int n, rank;
	if (MPI_Comm_size(MPI_COMM_WORLD, &n))
		perror("Error at MPI_Comm_size!\n");
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_Comm_rank!\n");

	MPI_Status status;


	double starttime, endtime;
	if (rank == 0)
	{
		starttime = MPI_Wtime();

		if (MPI_Send(buf_send, MAX_MESSAGE_SIZE, MPI_INT, 1, 1, MPI_COMM_WORLD))
			fprintf(stderr, "MPI_SEND ERROR\n");
		if (MPI_Recv(buf_recv, MAX_MESSAGE_SIZE, MPI_INT, 1, 1, MPI_COMM_WORLD, &status))
			fprintf(stderr, "MPI_RECV ERROR!\n");

		endtime = MPI_Wtime();

		printf("rank=%d, time (block #1): %f sec, array size: %d\n", rank, (endtime - starttime) / 2, MAX_MESSAGE_SIZE);
	}

	if (rank == 1)
	{

		int* buf_recv = new int[MAX_MESSAGE_SIZE];

		starttime = MPI_Wtime();

		if (MPI_Recv(buf_recv, MAX_MESSAGE_SIZE, MPI_INT, 0, 1, MPI_COMM_WORLD, &status))
			fprintf(stderr, "MPI_RECV ERROR!\n");
		if (MPI_Send(buf_recv, MAX_MESSAGE_SIZE, MPI_INT, 0, 1, MPI_COMM_WORLD))
			fprintf(stderr, "MPI_SEND ERROR\n");

		endtime = MPI_Wtime();

		printf("rank=%d, (time block #2): %f sec, array size: %d\n", rank, (endtime - starttime) / 2, MAX_MESSAGE_SIZE, buf_recv[MAX_MESSAGE_SIZE - 1]);
	}
	if (rank == 2)
	{	
		MPI_Request r1, r2;
		starttime = MPI_Wtime();

		if (MPI_Isend(buf_send, MAX_MESSAGE_SIZE, MPI_INT, 3, 1, MPI_COMM_WORLD, &r1))
			fprintf(stderr, "MPI_SEND ERROR\n");
		
		if (MPI_Irecv(buf_recv, MAX_MESSAGE_SIZE, MPI_INT, 3, 1, MPI_COMM_WORLD, &r2))
			fprintf(stderr, "MPI_SEND ERROR\n");

		
		MPI_Wait(&r1, &status);
		MPI_Wait(&r2, &status);
		endtime = MPI_Wtime();
		printf("rank=%d, time (not block1): %f sec, array size: %d\n", rank, (endtime - starttime) / 2, MAX_MESSAGE_SIZE);
		
	}
	if (rank == 3)
	{
		MPI_Request r1, r2;
		starttime = MPI_Wtime();

		int* buf_recv = new int[MAX_MESSAGE_SIZE];
		if (MPI_Irecv(buf_recv, MAX_MESSAGE_SIZE, MPI_INT, 2, 1, MPI_COMM_WORLD, &r1))
			fprintf(stderr, "MPI_SEND ERROR\n");

		if (MPI_Isend(buf_send, MAX_MESSAGE_SIZE, MPI_INT, 2, 1, MPI_COMM_WORLD, &r2))
			fprintf(stderr, "MPI_SEND ERROR\n");

		
		MPI_Wait(&r1, &status);
		MPI_Wait(&r2, &status);

		endtime = MPI_Wtime();
		printf("rank=%d, time non block2: %f sec, array size: %d\n", rank, (endtime - starttime) / 2, MAX_MESSAGE_SIZE, buf_recv[MAX_MESSAGE_SIZE-1]);


	}
	
	MPI_Finalize();
	return 0;
}
