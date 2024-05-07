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
	int rank,size;
	MPI_Init(&argc, &argv);
	MPI_Status a;
	MPI_Request req;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	char message_recv;
	char message_zero = char(int('0') + rank);
	//Ѕлокируема¤
	for (int i = 0; i < size; i++)
	{
		if (i == rank)
		{
			continue;
		}
		else 
		{
			MPI_Isend(&message_zero, 1, MPI_CHAR, i, 1, MPI_COMM_WORLD, &req);
			MPI_Recv(&message_recv, 1, MPI_CHAR, i, 1, MPI_COMM_WORLD, &a);
			printf("message for %d rank - '%c' ----- \n", rank, message_recv);
			//ѕечать вывода
		}
	}
	MPI_Finalize();
	return 0;
}


