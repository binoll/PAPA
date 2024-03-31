#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

int main(int argc, char* argv[])
{
	int rank, size;
	MPI_Init(&argc, &argv);
	MPI_Status a;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	char message_zero = char(int('0') + rank);
	char message_recv[size-1];
	//Ѕлокируема¤
	if (rank == 0) {
		for (int i = 0; i < size-1; i++)
		{
			MPI_Recv(&message_recv[i], 1, MPI_CHAR, i + 1, 1, MPI_COMM_WORLD, &a);
		}
		printf("===========================================\n");
		for (int i = 0; i < size-1; i++)
		{
			printf("message for 0 rank from %d rank - %c ----- \n", i+1, message_recv[i]);
		}
		printf("===========================================\n");
	}
	else {
		MPI_Send(&message_zero, 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
		printf("message from %d rank ----- %c ----- \n", rank,message_zero );
	}
	MPI_Finalize();
	return 0;
}


