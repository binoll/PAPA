#include "mpi.h"
#include "stdio.h"
#include <string>
#include <iostream>
#include "cstdlib"

#define MAX_MSG_SIZE 1024

using namespace std;

int main(int argc, char *argv[])
{
	int rank, allnum;
	
	MPI_Init(&argc, &argv);
	
	// Получаем ранг текущего процессора
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank)) perror("Err at Comm_rank :( \n");
	if (MPI_Comm_size(MPI_COMM_WORLD, &allnum)) perror("Err at Comm_size :( \n");
	
	int len = allnum;
	MPI_Request req[len];
	MPI_Status status[len];
	
	int *recvbuf = new int[len];
	int *sendbuf = new int[len];

	printf("\n");
	for (int i = 0; i < len; ++i)
	{
		sendbuf[i] = rank*len + i;
		recvbuf[i] = sendbuf[i];
		printf("%d ", recvbuf[i]);
	}
	printf("\n");
	
	for (int i = 0; i < len; ++i) 
	{
		MPI_Isend(&sendbuf[i], 1, MPI_INT, i, 1, MPI_COMM_WORLD, &req[i]);
		MPI_Irecv(&recvbuf[i], 1, MPI_INT, i, 1, MPI_COMM_WORLD, &req[i]);
	}
	MPI_Waitall(len, req, status);

	printf("\n");
	for (int i = 0; i < len; ++i)
	{
		printf("%d ", recvbuf[i]);
	}
	printf("\n");

	MPI_Finalize();
}