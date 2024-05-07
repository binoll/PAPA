////1.	С помощью неблокируемых операций точечного взаимодействия реализовать каждую 
////из функций коллективного взаимодействия
////MPI_Bcast
#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
	int n, rank;
	double start_1, end_1, start_2, end_2;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int count_1 = 0;
	int recvbuf_1;
	int recvbuf_2;
	int count_2 = 0;
	MPI_Request request[20];
	MPI_Status status[20];

	start_1 = MPI_Wtime();
	if (rank == 0)
	{
		recvbuf_2 = rank;
		for (int i = 1; i < n; i++)
		{
			MPI_Isend(&rank, 1, MPI_INT, i, 22, MPI_COMM_WORLD, &request[i - 1]);
		}
		MPI_Waitall(n - 1, request, status);
	}
	else
	{
		MPI_Irecv(&recvbuf_1, 1, MPI_INT, 0, 22, MPI_COMM_WORLD, &request[rank - 1]);
		MPI_Wait(&request[rank - 1], &status[rank - 1]);
		count_1 = recvbuf_1;
	}
	end_1 = MPI_Wtime();
	printf("MY_Bcast\nrank:%d\tmessage:%d\ttime:%f\n", rank, count_1, end_1 - start_1);
	
	start_2 = MPI_Wtime();
	MPI_Bcast(&recvbuf_2, 1, MPI_INT, 0, MPI_COMM_WORLD);
	end_2 = MPI_Wtime();
	printf("MPI_Bcast\nrank:%d\tmessage:%d\ttime:%f\n", rank, recvbuf_2, end_2 - start_2);
	MPI_Finalize();
	return 0;
}
