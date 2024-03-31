#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_MESSAGE_LEN 30

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);

	int myrank, namelen, process_count;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	MPI_Get_processor_name(processor_name, &namelen);

	int opt, bufsize;

	while ((opt = getopt(argc, argv, "s:")) != -1)
	{
		switch (opt)
		{
		case 's':
			bufsize = atoi(optarg);
			break;
		default:
			fprintf(stderr, "�������������: %s [-n process_id]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	char* sendbuf = new char[++bufsize];
	for (int i = 0; i < bufsize; ++i) sendbuf[i] = 'm';
	sendbuf[bufsize - 1] = '\0';

	double starttime, endtime;
	starttime = MPI_Wtime();

	if (myrank == 0)
	{ 
		MPI_Send(sendbuf, bufsize, MPI_INT, 1, 1, MPI_COMM_WORLD);

	}
	else if (myrank == 1)
	{
		char* recvbuf = new char[bufsize];
		MPI_Status status;

		MPI_Recv(recvbuf, bufsize, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

		endtime = MPI_Wtime();
		double diff_time = endtime - starttime;
		
		printf("The process with rank %d get message: ", myrank);
		for (int i = 0; i < bufsize; i++)
		{
			printf("%c", recvbuf[i]);
		}

		printf("\n");

		printf("Process rank: %d, That took %f seconds\n", myrank, diff_time);

		delete recvbuf;

	}

	MPI_Finalize();

}
