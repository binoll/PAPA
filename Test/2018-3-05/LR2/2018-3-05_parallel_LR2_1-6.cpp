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

	char** sendbuf = new char* [process_count];
	for (int i = 0; i < process_count; ++i) sendbuf[i] = new char[bufsize];
	for (int i = 0; i < process_count; ++i)
	{
		for (int j = 0; j < bufsize - 2; j++)
		{
			sendbuf[i][j] = char(int('a') + myrank);
		}
		sendbuf[i][bufsize - 2] = char(int('0') + i);
		sendbuf[i][bufsize - 1] = '\0';
	}

	char** recvbuf = new char* [process_count];
	for (int i = 0; i < process_count; i++) recvbuf[i] = new char[bufsize];

	MPI_Status status;
	MPI_Request request;

	double time_start_custom = MPI_Wtime();

	for (int i = 0; i < process_count; i++)
	{
		MPI_Isend(sendbuf[i], bufsize, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
		printf("Process with rank %d send process with rank %d message %s.\n", myrank, i, sendbuf[i]);
	}

	for (int i = 0; i < process_count; i++)
	{
		MPI_Irecv(recvbuf[i], bufsize, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

		printf("From process with rank %d process with rank %d GET MESSAGE %s.\n", i, myrank, recvbuf[i]);
	}

	double time_finish_custom = MPI_Wtime() - time_start_custom;

	if (myrank == 0) printf("Custom AlltoAll time is %f\n", time_finish_custom);


	char** sendbuf_mpi = new char* [process_count];
	for (int i = 0; i < process_count; ++i) sendbuf_mpi[i] = new char[bufsize];
	for (int i = 0; i < process_count; ++i)
	{
		for (int j = 0; j < bufsize - 2; j++)
		{
			sendbuf_mpi[i][j] = char(int('a') + myrank);
		}
		sendbuf_mpi[i][bufsize - 2] = char(int('0') + i);
		sendbuf_mpi[i][bufsize - 1] = '\0';
	}

	char** recvbuf_mpi = new char* [process_count];
	for (int i = 0; i < process_count; i++) recvbuf_mpi[i] = new char[bufsize];

	double time_start_mpi;
	time_start_mpi = MPI_Wtime();

	MPI_Alltoall(sendbuf_mpi, bufsize, MPI_CHAR, recvbuf_mpi, bufsize, MPI_CHAR, MPI_COMM_WORLD);

	double time_finish_mpi = MPI_Wtime() - time_start_mpi;

	for (int i = 0; i < process_count; i++)
	{
		printf("MPI: From process with rank %d process with rank %d GET MESSAGE %s.\n", i, myrank, recvbuf[i]);
	}

	if (myrank == 0) printf("MPI_AlltoAll time is %f\n", time_finish_mpi);

	MPI_Finalize();

	return 0;

}