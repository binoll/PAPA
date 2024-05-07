/*
3. Смоделировать последовательный двусторонний обмен сообщениями 
между двумя процессами, замерить с помощью функции MPI_Wtime() 
время на одну итерацию обмена, определить зависимость времени от 
длины сообщения.
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

	int n, rank, namelen;
	if (MPI_Comm_size(MPI_COMM_WORLD, &n))
		perror("Error at MPI_Comm_size!\n");
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_Comm_rank!\n");

	struct MPI_Status status;
	double starttime, endtime;

	if (rank == 0)
	{
		int* buf_send = new int[MAX_MESSAGE_SIZE];
		for (int i = 0; i < MAX_MESSAGE_SIZE; i++)
			buf_send[i] = i;
		
		int* buf_recv = new int[MAX_MESSAGE_SIZE];
		
		starttime = MPI_Wtime();

		if (MPI_Send(buf_send, MAX_MESSAGE_SIZE, MPI_INT, 1, 1, MPI_COMM_WORLD))
			fprintf(stderr, "MPI_SEND ERROR\n");
		if (MPI_Recv(buf_recv, MAX_MESSAGE_SIZE, MPI_INT, 1, 1, MPI_COMM_WORLD, &status))
			fprintf(stderr, "MPI_RECV ERROR!\n");
	
		endtime = MPI_Wtime();

		printf("rank=%d, time: %f sec, array size: %d\n", rank, (endtime - starttime) / 2, MAX_MESSAGE_SIZE);
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

		printf("rank=%d, time: %f sec, array size: %d, (test: %d)\n", rank, (endtime - starttime) / 2, MAX_MESSAGE_SIZE, buf_recv[MAX_MESSAGE_SIZE - 1]);
	}

	MPI_Finalize();
	return 0;
}
