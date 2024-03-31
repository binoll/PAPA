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

	char* sendbuf = new char[bufsize];
	for (int i = 0; i < bufsize - 1; ++i) sendbuf[i] = char(int('a') + myrank);
	sendbuf[bufsize - 1] = '\0';
	
	char** recvbuf = new char* [process_count];
	for (int i = 0; i < process_count; ++i) recvbuf[i] = new char[bufsize];

	double time_start_custom = MPI_Wtime();
	
	MPI_Isend(sendbuf, bufsize, MPI_CHAR, root, 1, MPI_COMM_WORLD, &request);
	
	if (myrank == root)
	{
		for (int i = 0; i < process_count; i++)
		{
			MPI_Irecv(recvbuf[i], bufsize, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);
			printf("From process with rank %d to process with rank %d , message: %s.\n", i, myrank, recvbuf[i]);
		}
	}

	double time_finish_custom = MPI_Wtime() - time_start_custom;
	if (myrank == 0) printf("Custom Bcast time is %f\n", time_finish_custom);

	char* sendbuf_mpi = new char[bufsize];
	//char* recvbuf = new char[bufsize];

	char** recvbuf_mpi = new char* [process_count];
	for (int i = 0; i < process_count; ++i) recvbuf_mpi[i] = new char[bufsize];

	for (int i = 0; i < bufsize - 1; ++i) sendbuf_mpi[i] = char(int('a') + myrank);
	sendbuf_mpi[bufsize - 1] = '\0';

	double time_start_mpi;
	time_start_mpi = MPI_Wtime();

	MPI_Gather(sendbuf_mpi, bufsize, MPI_CHAR, recvbuf_mpi, bufsize, MPI_CHAR, root, MPI_COMM_WORLD);

	double time_finish_mpi = MPI_Wtime() - time_start_mpi;

	if (myrank == root)
	{
		for (int i = 0; i < process_count; i++)
		{
			printf("MPI: From process with rank %d to process with rank %d , message: %s.\n", i, myrank, recvbuf[i]);
		}
	}
	if (myrank == 0) printf("MPI_Bcast time is %f\n", time_finish_mpi);

	MPI_Finalize();

	return 0;

}