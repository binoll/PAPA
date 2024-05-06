%%writefile task1_3.c
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
	send_buf = (int*)malloc((size + 1) * sizeof(int));
	int i = 0;
	while (i < size)
	{
		i++;
		send_buf[i-1] = rank + i;
	}
	printf("My rank = %d value: ", rank);
	i=0;
	while (i < size)
		{
			printf("%d ", send_buf[i]);
			i++;
		}
	printf("\n");
	
	int* count = NULL; 
	count = (int*)malloc(size * sizeof(int));
	i = 0;
	while (i < size)
	{
		count[i] = 1;
		i++;
	}

	int recv_buf;

	MPI_Reduce_scatter(send_buf, &recv_buf, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	printf("Rank:%d, recv_buf:%d\n", rank, recv_buf);
	free(send_buf);
	free(count);
	MPI_Finalize();
	return 0;
}
