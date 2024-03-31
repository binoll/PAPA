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
	char message_zero = '0';
	char message;
	MPI_Init(&argc, &argv);
	MPI_Status a;
	MPI_Request req;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//Ѕлокируема¤
	if (rank == 0) {
		MPI_Isend(&message_zero, 1, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &req);
		MPI_Irecv(&message, 1, MPI_CHAR, 3, 1, MPI_COMM_WORLD, &req);
		MPI_Wait(&req, &a);
		printf("massege for %d rank - %c ----- \n", rank, message);
	}
	else if (rank == size-1) {
		MPI_Irecv(&message, 1, MPI_CHAR, rank-1, 1, MPI_COMM_WORLD, &req);
		MPI_Wait(&req, &a);
		MPI_Isend(&message, 1, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &req);
		printf("massege for 3 rank - %c ----- \n", message);
	}
	else {
		MPI_Irecv(&message, 1, MPI_CHAR, rank-1, 1, MPI_COMM_WORLD, &req);
		MPI_Wait(&req, &a);
		MPI_Isend(&message, 1, MPI_CHAR, rank+1, 1, MPI_COMM_WORLD, &req);
		printf("massege for 1 rank - %c ----- \n", message);
	}
	MPI_Finalize();
	return 0;
}


