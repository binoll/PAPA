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
	double starttime, endtime, starttime2, endtime2;
	int opt, size, n, rank, namelen;
	while ((opt = getopt(argc, argv, "r:")) != -1) {
		switch (opt) {
		case 'r':
			size = atoi(optarg);
			break;
		default:
			fprintf(stderr, "»спользование: %s [-n process_id]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	int* message_zero = new int[size];
	for (int i = 0; i < size; i++) {
		message_zero[i] = rank;
	}

	int* message_first_recv = new int[size];
	int* message_third_recv = new int[size];
	MPI_Init(&argc, &argv);
	MPI_Status a;
	MPI_Request req;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//Ѕлокируема¤
	starttime = MPI_Wtime();
	starttime2 = MPI_Wtime();
	if (rank == 0) {

		MPI_Send(message_zero, size, MPI_INT, 1, 1, MPI_COMM_WORLD);
	}
	else if (rank == 1) {
		MPI_Recv(message_first_recv, size, MPI_INT, 0, 1, MPI_COMM_WORLD, &a);
		endtime = MPI_Wtime();
		printf("\n massege for 1 rank  ");
		for (int i = 0; i < size; i++)
		{
			printf("| %d |", message_first_recv[i]);
		}
		printf("\n ----- time = %f \n", endtime - starttime);

	}
	else if (rank == 2) {
		MPI_Isend(message_zero, size, MPI_INT, 3, 1, MPI_COMM_WORLD, &req);
	}
	else if (rank == 3) {
		MPI_Irecv(message_third_recv, size, MPI_INT, 2, 1, MPI_COMM_WORLD, &req);
		MPI_Wait(&req,&a);
		endtime2 = MPI_Wtime();
		printf("\n massege for 1 rank  ");
		for (int i = 0; i < size; i++)
		{
			printf("| %d |", message_first_recv[i]);
		}
		printf("\n ----- time = %f \n", endtime2 - starttime2);
	}
	MPI_Finalize();
	return 0;
}


