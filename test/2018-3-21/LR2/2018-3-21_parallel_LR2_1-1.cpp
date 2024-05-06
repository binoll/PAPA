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

	int* message_first_recv = new int[size];
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Request req_a[n];
	MPI_Status a[n];
	starttime = MPI_Wtime();
	if (rank == 0) {
		int* message_zero = new int[size];
		for (int i = 0; i < size; i++) {
			message_zero[i] = rank;
		}
		for (int i = 1, re_i = 1; i < n; i++)
		{

			MPI_Isend(message_zero, size, MPI_INT, i, 1, MPI_COMM_WORLD, &req_a[re_i]);
			re_i++;
		}
		printf("I send all, message");
	}
	else
	{
		for (int i = 1; i < n; i++)
		{
			if (rank == i) {
				MPI_Irecv(message_first_recv, size, MPI_INT, 0, 1, MPI_COMM_WORLD, &req_a[i]);
				MPI_Wait(&req_a[i], &a[i]);
				endtime = MPI_Wtime();
			}
		}
		printf("\n massege for %d rank  ", rank);
		for (int i = 0; i < size; i++)
		{
			printf("| %d |", message_first_recv[i]);
		}
		printf("\n ----- time = %f \n", endtime - starttime);
		printf("I recv message. My Rank is %d", rank);
	}
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

	int* message_first_recv = new int[size];
	MPI_Init(&argc, &argv);
	MPI_Status a;
	MPI_Request req;
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int* message_zero = new int[size];
	//�����������
	starttime = MPI_Wtime();
	if (rank == 0) {
		int* message_zero = new int[size];
		for (int i = 0; i < size; i++) {
			message_zero[i] = rank + i;
		}
	}
	MPI_Bcast(message_zero, 1, MPI_INT, 0, MPI_COMM_WORLD);
	endtime = MPI_Wtime();
	printf("\n ----- time = %f \n", endtime - starttime);
	MPI_Finalize();
	return 0;
}
