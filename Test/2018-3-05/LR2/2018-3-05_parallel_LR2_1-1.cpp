#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

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
	char* recvbuf = new char[++bufsize];

	for (int i = 0; i < bufsize; ++i) sendbuf[i] = 'm';
	sendbuf[bufsize - 1] = '\0';

	MPI_Status status[1];
	MPI_Request request[1];

	double starttime, endtime;
	starttime = MPI_Wtime();

	if (myrank == 0)
	{	
		sleep(2);
		MPI_Isend(sendbuf, bufsize, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &request[0]);

		recvbuf = sendbuf;
	}
	else if (myrank == 1)
	{
		MPI_Irecv(recvbuf, bufsize, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &request[0]);
	}

	MPI_Waitall(1, request, status);
	endtime = MPI_Wtime();

	printf("Process with rank %d , message: %s. That took %f seconds\n", myrank, sendbuf, endtime - starttime);

	MPI_Finalize();

	return 0;

}
