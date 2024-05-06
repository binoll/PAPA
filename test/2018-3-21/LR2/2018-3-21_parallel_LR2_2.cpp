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
	int opt, size_x, size_y, n, rank;
	while ((opt = getopt(argc, argv, "r:d:")) != -1) {
		switch (opt) {
		case 'r':
			size_x = atoi(optarg);
			break;
		case 'd':
			size_y = atoi(optarg);
			break;
		default:
			fprintf(stderr, "�������������: %s [-n process_id]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status a[n];
	MPI_Request req[n];
	MPI_Request req_1[n];
	//�������� ������� � �����  
	int* message_zero = new int[size_x];
	int* message_first = new int[size_y];
	int* message = new int[size_x * size_y];
	for (int x = 0; x < size_x; x++) {
		message_zero[x] = x;
		printf("| %d |", x,message_zero[x]);
	}
	printf("- 1. rank = %d\n", rank);
	MPI_Barrier(MPI_COMM_WORLD);
	for (int y = 0; y < size_y; y++) {
		message_first[y] = y;
		printf("| %d |", y, message_first[y]);
	}
	printf("- 2. rank = %d\n", rank);
	//��������
	MPI_Barrier(MPI_COMM_WORLD);
	for (int i = 0; i < size_x; i++)
	{
		for (int j = 0; j < size_y; j++) {
			message[j + i * size_y] = message_zero[i] * message_first[j];
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	//���� ������ �� ����� ��������
	int* message_recv = new int[size_x * size_y * n];
	MPI_Gather(message, size_x * size_y, MPI_INT,message_recv, size_x * size_y,MPI_INT,0, MPI_COMM_WORLD);

	if (rank == 0){ 
		for (int j = 0; j < size_x * size_y * n; j++) {
		}
		int* res = new int[size_x * size_y];
		for (int j = 0; j < size_x * size_y; j++) {
			res[j] = 0;
		}
		//�������� �������� ������
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < size_x*size_y; j++) {
				res[j] += message_recv[size_x * size_y * i + j];
			}
		}
		//�������� ����� �������	
		for (int i = 0; i < size_x * size_y; i++) {
			if (i % size_y == 0) {
				printf("\n");
			}
			printf("[ %d ]", res[i]);
		}
	}
	MPI_Finalize();
	return 0;
}
