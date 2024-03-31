#include <stdio.h> 
#include <mpi.h>   
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{

	int rank, allnum;
	
	MPI_Init(&argc, &argv);
	
	if (MPI_Comm_rank( MPI_COMM_WORLD, &rank)) perror("Err at Comm_rank :( \n");
	if (MPI_Comm_size(MPI_COMM_WORLD, &allnum)) perror("Err at Comm_size :( \n");

	int* sendbuff = NULL;
	sendbuff = new int[allnum+1];
	
	for (int i = 0; i < allnum+1; ++i) sendbuff[i] = rank*allnum + i;
	
	printf("Rank %d, sendbuff: ", rank);
	for (int i = 0; i < allnum + 1; ++i) printf(" %d", sendbuff[i]);
	printf("\n");

	int* count = NULL; 
	count = new int[allnum];
	for (int i = 0; i < allnum; ++i) count[i] = 1;
	count[allnum - 2] = 2;
	
	int recvbuff[count[rank]];
	MPI_Reduce_scatter(sendbuff, &recvbuff, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	printf("Rank: %d, recvbuff: ", rank);
	for (int i = 0; i < count[rank]; ++i) printf("%d ", recvbuff[i]);
	printf("\n");
		
	MPI_Finalize();
	return 0;
}