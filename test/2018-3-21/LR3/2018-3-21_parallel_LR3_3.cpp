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
	int opt, N, N_C, n, rank;
	while ((opt = getopt(argc, argv, "r:d:")) != -1) {
		switch (opt) {
		case 'r':
			N = atoi(optarg);
			break;
		case 'd':
			N_C = atoi(optarg);
			break;
		default:
			fprintf(stderr, "�������������: %s [-n process_id]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	MPI_Init(&argc, &argv);
	// �������� ���������� ��� ������

	int position;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n);

	int a[N];
	char b[N];
	char c[N_C];

	if (rank == 0)
	{
		//������� � ���������� ��������
		position = 0;
		for (int i = 0; i < N; i++)
		{
			a[i] = n * rank + i;
			b[i] = char(int('A') + i);
		}
		printf("PACK rank=%d, char array: \"%.5s\";\n int array: ", rank, b);
		for (int i = 0; i < N; i++) {
			printf("%d ", a[i]);
		}
		printf("\n\n\n\n\n");
		//�������� 
		MPI_Pack(a, N, MPI_INT, c, N_C, &position, MPI_COMM_WORLD);
		MPI_Pack(b, N, MPI_CHAR, c, N_C, &position, MPI_COMM_WORLD);
	}
	// ��������
	MPI_Bcast(c, N_C, MPI_PACKED, 0, MPI_COMM_WORLD);

	if (rank == 1) {
		position = 0;
		MPI_Unpack(c, N_C, &position, a, N, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(c, N_C, &position, b, N, MPI_CHAR, MPI_COMM_WORLD);

		printf("UNPACK rank=%d, char array: \"%.5s\";\n int array: ", rank, b);
		for (int i = 0; i < N; i++)
			printf("%d ", a[i]);
		printf("\n");
	}
	MPI_Finalize();
	return 0;
}
