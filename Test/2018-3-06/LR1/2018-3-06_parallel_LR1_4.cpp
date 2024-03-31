#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "Russian");
	int rank, size;
	double starttime, endtime;
	size = strlen(argv[1]);
	char* buf = new char[size];
	buf = argv[1];
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char* sbuf = new char[size];
	char* rbuf = new char[size];
	starttime = MPI_Wtime();
	MPI_Request req;
	MPI_Status status;
	if (rank == 0)
	{
		sbuf = buf;
		MPI_Isend(sbuf, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &req);
	}
	if (rank == 1)
	{
		MPI_Irecv(rbuf, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &req);
		MPI_Wait(&req, &status);
		printf("1 process have - %s\n", rbuf);
		endtime = MPI_Wtime();
		printf("Time one iteration = %f\n", endtime - starttime);
	}
	MPI_Finalize();
}
