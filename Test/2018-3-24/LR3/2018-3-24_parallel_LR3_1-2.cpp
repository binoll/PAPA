#include <stdio.h>   
#include <mpi.h>     
#include <unistd.h>
#include <stdlib.h>
#define ARRAYSIZE 3

int main(int argc, char* argv[])
{
	int rank, allnum;
	
	MPI_Init(&argc, &argv);
	
	if (MPI_Comm_rank( MPI_COMM_WORLD, &rank)) perror("Err at Comm_rank :( \n");
	if (MPI_Comm_size(MPI_COMM_WORLD, &allnum)) perror("Err at Comm_size :( \n");
	
	int sendbuff[ARRAYSIZE];
	int recvbuff[ARRAYSIZE];
	
	for (int i = 0; i < ARRAYSIZE; ++i) sendbuff[i] = rank*allnum + i;
	
	printf("Rank %d, sendbuff: ", rank);
	for (int i = 0; i < ARRAYSIZE; ++i) printf(" %d", sendbuff[i]);
	printf("\n");
	
	MPI_Allreduce(sendbuff, recvbuff, ARRAYSIZE, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	
	printf("\n");
	printf("!Rank %d, recvbuff: ", rank);
	for (int i = 0; i < ARRAYSIZE; ++i) printf(" %d", recvbuff[i]);
	printf("\n");
	
	MPI_Finalize();

	return 0;
}