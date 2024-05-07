#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	int rank, opt, size, tag = 0;
	double starttime, endtime;
	
	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	while ((opt = getopt(argc, argv, "s:")) != -1)
	{
		switch (opt)
		{
		case 's':
			size = atoi(optarg);
			break;
		default:
			printf("NONONO, repeat with normal argument");
		}
	}
	int* buf = new int[size + 1];
	
	starttime = MPI_Wtime();
	if (rank == 0) {
		MPI_Send(buf, size+1, MPI_INT, 1, tag, MPI_COMM_WORLD);
	}
	else if (rank == 1) {
		MPI_Recv(buf, size+1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
	}
	printf("I have gift for %d process \n" , rank);
	endtime = MPI_Wtime();
	printf("Time one iteration = %f \n", endtime - starttime);
	MPI_Finalize();
	}