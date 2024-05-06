#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char** argv)
{
	double starttime, endtime;
	int opt, N, n, rank;
	while ((opt = getopt(argc, argv, "r:")) != -1) {
		switch (opt) {
		case 'r':
			N = atoi(optarg);
			break;
		default:
			fprintf(stderr, "�������������: %s [-n process_id]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Datatype xpose;
	// �������� ����������� ����� ������ column � xpose
	MPI_Type_vector(N, N, N, MPI_INT, &xpose);
	MPI_Type_commit(&xpose);

	//�������� � ���������� ������������� �������
	int buff1[N][N];


	//����� ����. �������
	if (rank == 0) {
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				buff1[i][j] = rand() % 10 + 1;
			}
		}
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				printf("| %d |", buff1[i][j]);
			}
			printf("\n");
		}
	}
	printf("\n\n\n");
	// �������� ������ � ����� ������
	MPI_Bcast(&buff1[0][0], 1, xpose, 0, MPI_COMM_WORLD);

	if (rank == 1) {
		for (int i = 0; i < N; i++)
		{

			for (int j = 0; j < N; j++)
			{
				printf("| %d |", buff1[i][j]);
			}
			printf("\n");
		}
	}
	MPI_Finalize();
	return 0;
}