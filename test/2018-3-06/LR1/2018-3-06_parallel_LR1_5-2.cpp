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
	char* sbuf = new char[size];
	char* rbuf = new char[size];
	sbuf = buf;

	if (rank == 0)
	{
		for (int i = 1; i < n; i++)
		{
			MPI_Send(sbuf, size, MPI_CHAR, i, tag, MPI_COMM_WORLD);
			MPI_Recv(rbuf, size, MPI_CHAR, i, tag, MPI_COMM_WORLD, &status);
			printf("\n Master recv froms %d: %s \n", i, rbuf);
		}
	}
	else
	{
		MPI_Send(sbuf, size, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
		MPI_Recv(rbuf, size, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
		printf("\n %d recv froms 0: %s \n", rank , rbuf);
	}
	MPI_Finalize;
}
