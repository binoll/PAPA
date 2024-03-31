#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

int main(int argc, char* argv[])
{
	double starttime, endtime;
	int opt, size, n, rank;
	while ((opt = getopt(argc, argv, "r:")) != -1) {
		switch (opt) {
		case 'r':
			size = atoi(optarg);
			break;
		default:
			fprintf(stderr, "�������������: %s [-n process_id]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status a;
	MPI_Request req_not_root;
	starttime = MPI_Wtime();
	if (rank == 0) {
		MPI_Request req[n];
		//�������� � ����� �������������
		int* message_zero = new int[size*n];
		for (int y = 0; y < n * size; y++) {
			message_zero[y] = rank;
			printf(" %d ", message_zero[y]);
		}
		for (int i = 0; i < n; i++)
		{
			MPI_Isend(&message_zero[i*size], size, MPI_INT, i, 1, MPI_COMM_WORLD, &req[i]);
		}
	}
	//���������
	int* message_first_recv = new int[size];
	MPI_Irecv(message_first_recv, size, MPI_INT, 0, 1, MPI_COMM_WORLD, &req_not_root);
	MPI_Wait(&req_not_root, &a);
	endtime = MPI_Wtime();
	//����� ������
	printf("Rank - %d:\n", rank);
	for (int j = 0; j < size; j++)
	{
		printf("| %d |", message_first_recv[j]);
	}
	printf("\n ----- time = %f \n", endtime - starttime);

	

	MPI_Finalize();
	return 0;
}

int main(int argc, char* argv[])
{
	double starttime, endtime;
	int opt, size, n, rank;
	while ((opt = getopt(argc, argv, "r:")) != -1) {
		switch (opt) {
		case 'r':
			size = atoi(optarg);
			break;
		default:
			fprintf(stderr, "�������������: %s [-n process_id]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status a;
	MPI_Request req_not_root;
	starttime = MPI_Wtime();
	MPI_Request req[n];
	//�������� � ����� �������������
	int* message_zero = new int[size * n];
	int* message_first_recv = new int[size];
	for (int y = 0; y < n * size; y++) {
		message_zero[y] = rank;
		printf(" %d ", message_zero[y]);
	}
	//���������
	MPI_Scatter(message_zero,size,MPI_INT,message_first_recv,size,MPI_INT,0, MPI_COMM_WORLD);
	endtime = MPI_Wtime();
	//����� ������
	printf("Rank - %d:\n", rank);
	for (int j = 0; j < size; j++)
	{
		printf("| %d |", message_first_recv[j]);
	}
	printf("\n ----- time = %f \n", endtime - starttime);

	

	MPI_Finalize();
	return 0;
}


