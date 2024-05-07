%%writefile task1_4.c
#include <stdio.h>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	int* send_buf = NULL;
	send_buf = (int*)malloc(size * sizeof(int));
	int i = 0;
	while (i < size)
	{
		send_buf[i] = rank + i;
		i++;
	}
	printf("My rank = %d value: ", rank);
	i=0;
	while (i < size)
		{
			printf("%d ", send_buf[i]);
			i++;
		}
	printf("\n");
	MPI_Barrier( MPI_COMM_WORLD);
	int* recv_buf = NULL;
	recv_buf = (int*)malloc(size * sizeof(int));

	MPI_Scan(send_buf, recv_buf, size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	free(send_buf);
	i = 0;
	printf("Rank:%d, recv_buf:", rank);
	while(i < size)
	{
		printf("%d ", recv_buf[i]);
		i++;
	}
	printf("\n");
	free(recv_buf);
	MPI_Finalize();
	return 0;
}