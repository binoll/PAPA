/*
Разработать параллельную программу, 
демонстрирующую применение MPI_Reduce.
*/

#include <stdio.h>   // printf()
#include <mpi.h>     // MPI functions
#include <unistd.h>
#include <stdlib.h>
#define SIZE_ARRAY 3

int main(int argc, char* argv[])
{

	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int send_buf[SIZE_ARRAY];
	int i = 0;
	while (i < SIZE_ARRAY)
	{
		send_buf[i] = rank + i;
		i++;
	}
	int recv_buf[SIZE_ARRAY];
	MPI_Reduce(send_buf, recv_buf, SIZE_ARRAY, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if (rank == 0)
	{
		printf("Rank 0, recv_buf: ");
		i = 0;
		while (i < SIZE_ARRAY)
		{
			printf("%d ", recv_buf[i]);
			i++;
		}
		printf("\n");
	}
	MPI_Finalize();

	return 0;
}