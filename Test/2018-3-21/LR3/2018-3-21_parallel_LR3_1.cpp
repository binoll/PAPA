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
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	int* count = new int[n];
	for (int i = 0; i < n; i++)
	{
		count[i] = 1;
	}
	//�������� � ���������� ������ ����������
	int* buff = new int[n * size];
	for (int i = 0; i < n * size; i++)
	{
		buff[i] = rank;
	}
	for (int i = 0; i < n * size; i++)
	{
		printf("%d ", buff[i]);
	}

	int* recv_buff = new int[n * size];
	int* all_recv_buff = new int[n * size];
	int* red_sc_recv_buff = new int[n * size];
	int* scan_buf = new int[n * size];
	MPI_Barrier(MPI_COMM_WORLD);
	printf("\n");
	MPI_Reduce(buff, recv_buff, n * size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if (rank == 0) {
		for (int i = 0; i < n * size; i++)
		{
			printf("%d ", recv_buff[i]);
		}
		printf("Reduce\n");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Allreduce(buff, all_recv_buff, n * size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	for (int i = 0; i < n * size; i++)
	{
		printf("%d ", all_recv_buff[i]);
	}
	printf("Allreduce\n");
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce_scatter(buff, red_sc_recv_buff, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	printf("%d ", red_sc_recv_buff[0]);
	printf("Reduce_scatter\n");
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Scan(buff, scan_buf, n * size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	for (int i = 0; i < n * size; i++)
	{
		printf("%d ", scan_buf[i]);
	}
	printf("Scan\n");
	MPI_Finalize();
}


