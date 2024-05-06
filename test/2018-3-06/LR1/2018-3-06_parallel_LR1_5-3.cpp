#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	int rank, size, n, tag = 322;
	size = strlen(argv[1]);
	char* buf = new char[size];
	buf = argv[1];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Status status;
	MPI_Request req;

	char* sbuf = new char[size];
	char* rbuf = new char[size];
	sbuf = buf;

	
	for (int i = 0; i < n; i++)
	{
		if (i == rank)
			continue;
		MPI_Isend(sbuf, size, MPI_CHAR, i, tag, MPI_COMM_WORLD, &req);
		MPI_Irecv(rbuf, size, MPI_CHAR, i, tag, MPI_COMM_WORLD, &req);
		printf("\n rank=%d from rank=%d: %s \n", rank, i, rbuf);
	}
	
	MPI_Finalize;
}
