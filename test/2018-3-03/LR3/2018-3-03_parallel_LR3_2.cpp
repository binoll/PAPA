#include "mpi.h"
#include <stdio.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int opt, size;
    while ((opt = getopt(argc, argv, "s:")) != -1)
    {
        switch (opt)
        {
        case 's':
            size = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Bad argument");
            exit(EXIT_FAILURE);
        }
    }

    int n, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    

    float a[size][size];
    MPI_Aint sizeoffloat;

    MPI_Datatype column, xpose;
    MPI_Type_extent(MPI_FLOAT, &sizeoffloat);
    MPI_Type_vector(size, 1, size, MPI_FLOAT, &column);
    MPI_Type_hvector(size, 1, sizeoffloat, column, &xpose);
    MPI_Type_commit(&xpose);



    if (rank == 0) 
    {
        cout << "rank "<< rank << " sent matrix" << endl;
        for (int i = 0; i < size; ++i)
        {
            for (int j = 0; j < size; ++j)
            {
                a[i][j] = (rand() % 1000)/10;
                cout << a[i][j] << " ";
            }

        cout << endl;
 	}
    }

    MPI_Bcast(&a, 1, xpose, 0, MPI_COMM_WORLD);

    if (rank != 0) 
    {
        for (int i = 0; i < size; ++i)
        {
            cout << "rank " << rank << " got matrix" << endl;
            for (int j = 0; j < size; ++j)
                cout << a[i][j] << " ";
            cout << endl;
        }
    }

    MPI_Finalize();
}