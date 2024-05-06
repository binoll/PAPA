//1.	С помощью неблокируемых операций точечного взаимодействия реализовать каждую 
//из функций коллективного взаимодействия
//(MPI_Barrier
#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
	int n, rank;
	double start_1, end_1, start_2, end_2;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int count_1 = 0;
	int count_2 = 0;
	int recvbuf_1;
	int recvbuf_2;
	MPI_Request request[2];
	MPI_Status status[2];
	MPI_Request req[20];
	MPI_Status stat[20];

	start_1 = MPI_Wtime();
	for (int i = 0; i < n; i++)
	{
		MPI_Isend(&rank, 1, MPI_INT, i, 22, MPI_COMM_WORLD, &request[0]);
		MPI_Irecv(&recvbuf_1, 1, MPI_INT, i, 22, MPI_COMM_WORLD, &request[1]);
		MPI_Waitall(2, request, status);
		count_1 += recvbuf_1;
	}
	end_1 = MPI_Wtime();
	printf("MY_Barrier\nrank:%d\tcount:%d\ttime:%f\n", rank, count_1, end_1-start_1);

	start_2 = MPI_Wtime();
	for (int i = 0; i < n; i++)
	{
		MPI_Isend(&rank, 1, MPI_INT, i, 22, MPI_COMM_WORLD, &req[i]);
		MPI_Irecv(&recvbuf_2, 1, MPI_INT, i, 22, MPI_COMM_WORLD, &req[i + n]);
		MPI_Barrier(MPI_COMM_WORLD);
		count_2 += recvbuf_2;
	}
	end_2 = MPI_Wtime();
	printf("MPI_Barrier\nrank:%d\tcount:%d\ttime:%f\n", rank, count_2, end_2-start_2);

	MPI_Finalize();
	return 0;
}