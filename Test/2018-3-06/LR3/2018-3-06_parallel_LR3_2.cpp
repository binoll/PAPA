#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>  

int main(int argc, char **argv)
{
	srand(time(NULL));
	MPI_Init(&argc, &argv);
	
	int size,rank,i,j;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	//int
	int int_buf_send[size];
	int int_buf_recv[size];
	
	for (int i=0;i<size;i++)
		int_buf_send[i] = (rank*10) + (rand() % 10);
	
	MPI_Allreduce(int_buf_send,int_buf_recv, size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	
	
	for (int i=0;i<size;i++)
		printf("%d ", int_buf_recv[i]);
	printf("\n");

	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	//char

	char char_buf_send[size];
	char char_buf_recv[size];
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0)
		printf("\n");
	
	
	for (int i=0;i<size;i++)
		char_buf_send[i] = (rand() % 24) + 65;
	
	MPI_Allreduce(char_buf_send,char_buf_recv, size, MPI_CHAR, MPI_BXOR, MPI_COMM_WORLD);
	

	for (int i=0;i<size;i++)
		printf("%c ", char_buf_recv[i]);
	printf("\n");

	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	//double

	double double_buf_send[size];
	double double_buf_recv[size];
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0)
		printf("\n");
	
	double add_point = rand() % 10;
	for (int i=0;i<size;i++)
		double_buf_send[i] = (rank*10) + (rand() % 10) + add_point/10;
	
	MPI_Allreduce(double_buf_send,double_buf_recv, size, MPI_DOUBLE, MPI_PROD, MPI_COMM_WORLD);
	

	for (int i=0;i<size;i++)
		printf("%f ", double_buf_recv[i]);
	printf("\n");
	
	
	MPI_Finalize();
}
