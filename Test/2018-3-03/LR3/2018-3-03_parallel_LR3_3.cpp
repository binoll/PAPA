#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{

    int rank,packsize, position;
    int a;
    double b;
    int size = 100, count = 1;
    char packbuf[size];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == 0)
    {
	a = 4;
	b = 1.234;
	position = 0;
	MPI_Pack(&a, count, MPI_INT, packbuf, size, &position, MPI_COMM_WORLD);
	MPI_Pack(&b, count, MPI_DOUBLE, packbuf, size, &position, MPI_COMM_WORLD);
    }    
    
    MPI_Bcast(packbuf, size, MPI_PACKED, 0, MPI_COMM_WORLD);
    
    if (rank != 0)
    {
	position = 0;
	MPI_Unpack(packbuf, size, &position, &a, count, MPI_INT, MPI_COMM_WORLD);
	MPI_Unpack(packbuf, size, &position, &b, count, MPI_DOUBLE, MPI_COMM_WORLD);
    }
    if (rank == 0)
        cout << "Process " << rank << " packed " << a << " (int) and " << b << " (double)" << endl;
    else
	cout << "Process " << rank << " unpacked " << a << " (int) and " << b << " (double)" << endl;
    MPI_Finalize();
}