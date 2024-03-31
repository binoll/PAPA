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

	//�������� ����� ���������
	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	// �������� ���� �������� ����������    
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	// �������� ��� ��
	MPI_Get_processor_name(processor_name, &namelen);

	int opt, bufsize, root;

	while ((opt = getopt(argc, argv, "s:r:")) != -1)
	{
		switch (opt)
		{
		case 's':
			bufsize = atoi(optarg);
			break;
		case 'r':
			root = atoi(optarg);
			break;
		default:
			fprintf(stderr, "�������������: %s [-n process_id]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	MPI_Status status;
	MPI_Request request;

	char** sendbuf = new char* [process_count];
	for (int i = 0; i < process_count; ++i)
	{
		sendbuf[i] = new char[bufsize];
		for (int j = 0; j < bufsize - 1; ++j) sendbuf[i][j] = char(int('a') + i);
		sendbuf[i][bufsize - 1] = '\0';
	}

	char* recvbuf = new char[bufsize];

	double time_start_custom = MPI_Wtime();

	if (myrank == root)
	{
		for (int i = 0; i < process_count; i++)
		{
			MPI_Isend(sendbuf[i], bufsize, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
		}
	}

	MPI_Irecv(recvbuf, bufsize, MPI_CHAR, root, 1, MPI_COMM_WORLD, &request);
	MPI_Wait(&request, &status);

	double time_finish_custom = MPI_Wtime() - time_start_custom;
	if (myrank == root) printf("Custom Bcast time is %f\n", time_finish_custom);

	printf("From process with rank %d to process with rank %d , message: %s.\n", root, myrank, recvbuf);



	char** sendbuf_mpi = new char* [process_count];
	for (int i = 0; i < process_count; ++i)
	{
		sendbuf_mpi[i] = new char[bufsize];
		for (int j = 0; j < bufsize - 1; ++j) sendbuf_mpi[i][j] = char(int('a') + i);
		sendbuf_mpi[i][bufsize - 1] = '\0';
	}

	char* recvbuf_mpi = new char[bufsize];

	double time_start_mpi;
	time_start_mpi = MPI_Wtime();

	MPI_Scatter(sendbuf_mpi, bufsize, MPI_CHAR, recvbuf_mpi, bufsize, MPI_CHAR, root, MPI_COMM_WORLD);

	double time_finish_mpi = MPI_Wtime() - time_start_mpi;
	
	printf("MPI: From process with rank %d to process with rank %d , message: %s.\n", root, myrank, recvbuf);
	
	
	if (myrank == 0) printf("MPI_Bcast time is %f\n", time_finish_mpi);

	MPI_Finalize();

	return 0;

}
