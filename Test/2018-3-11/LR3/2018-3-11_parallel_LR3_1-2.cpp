%%writefile task1_2.c
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#define SIZE_ARRAY 5

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int send_buf[SIZE_ARRAY];
	int i = 0;
	while (i < SIZE_ARRAY)
	{
		send_buf[i] = rank + i;
		i++;
	}
	printf("My rank = %d value: ", rank);
	i=0;
	while (i < SIZE_ARRAY)
		{
			printf("%d ", send_buf[i]);
			i++;
		}
		printf("\n");
	int recv_buf[SIZE_ARRAY];
	MPI_Allreduce(send_buf, recv_buf, SIZE_ARRAY, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	i = 0;

	printf("Rank %d, recv_buf: ", rank);
	while (i < SIZE_ARRAY)
	{
		printf("%d ", recv_buf[i]);
		i++;
	}
	printf("\n");
	MPI_Finalize();
	return 0;
}
