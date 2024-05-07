#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MPI_SEND_COUNT 1
#define MPI_RECV_COUNT 1


using namespace std;

int main(int argc, char* argv[])
{
	int rank, size;
	char message_recv;
	MPI_Init(&argc, &argv);
	MPI_Status a;
	MPI_Request req;
	int sendtag = 1;
	int recvtag = 1;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	char message_zero = char(int('0') + rank);
	//Ѕлокируема¤
	if (rank == 0) {
		MPI_Sendrecv(&message_zero, MPI_SEND_COUNT, MPI_CHAR, 1,sendtag, 
			&message_recv, MPI_RECV_COUNT, MPI_CHAR, 3, recvtag,MPI_COMM_WORLD, &a);
		printf("message for 0 rank from 3 - %c ----- \n", message_recv);
	}
	else if (rank == size-1) {
		MPI_Sendrecv(&message_zero, MPI_SEND_COUNT, MPI_CHAR, 0, sendtag,
			&message_recv, MPI_RECV_COUNT, MPI_CHAR, rank-1, recvtag, MPI_COMM_WORLD, &a);
		printf("message for %d rank from  %d - %c ----- \n", rank, rank-1, message_recv);
	}
	else {
		MPI_Sendrecv(&message_zero, MPI_SEND_COUNT, MPI_CHAR, rank+1, sendtag,
			&message_recv, MPI_RECV_COUNT, MPI_CHAR, rank-1, recvtag, MPI_COMM_WORLD, &a);
		printf("message for %d rank from %d - %c ----- \n", rank, rank-1, message_recv);
	}
	MPI_Finalize();
	return 0;
}


