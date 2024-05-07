#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int sendbuf[size];
	int recvbuf[size];
	printf("rank:%d\tsendbuf: ", rank);
	for (int i = 0; i < size ; i++)
	{
		sendbuf[i] = i;
		printf("%d ", sendbuf[i]);
	}
	printf("\n");
	/*int count[3];
	count[0] = 1;
	count[1] = 2;
	count[2] = 1;*/                                            
	int count[size];
	for (int i = 0; i < size; i++)
	{
		count[i] = 1;
	}
	MPI_Reduce_scatter(sendbuf, recvbuf, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    printf("rank:%d\trecvbuf: ", rank);
    printf("%d ", recvbuf[0]);
    printf("\n");
	MPI_Finalize();
	return 0;
}