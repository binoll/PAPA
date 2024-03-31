/*
Разработать параллельную программу, 
демонстрирующую применение MPI_Reduce_scatter.
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
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int* send_buf = NULL;
	send_buf = (int*)malloc((size + 1) * sizeof(int));
	int i = 0;
	while (i < size + 1)
	{
		send_buf[i] = rank + i;
		i++;
	}

	int* count = NULL; 
	count = (int*)malloc(size * sizeof(int));
	i = 0;
	while (i < size)
	{
		count[i] = 1;
		i++;
	}
	count[size - 1] = 2;
	if (rank == size - 1)
	{
		int recv_buf[2];
		MPI_Reduce_scatter(send_buf, &recv_buf, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
		printf("Rank:%d, recv_buf:%d %d\n", rank, recv_buf[0], recv_buf[1]);
	}
	else
	{
		int recv_buf;
		MPI_Reduce_scatter(send_buf, &recv_buf, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
		printf("Rank:%d, recv_buf:%d\n", rank, recv_buf);
	}
	free(send_buf);
	free(count);
	

	MPI_Finalize();
	return 0;
}