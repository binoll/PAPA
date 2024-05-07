#include "mpi.h"
#include <stdio.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int tag = 0;
    MPI_Status *status = new MPI_Status[size];
    MPI_Request *requests = new MPI_Request[size];
    int sendbuf = rank;
    int *recvbuf = new int[size];
    
    cout << "Rank is " << rank << " send " << sendbuf << endl;
    
    int i = 0;
    for (i; i < size; ++i)
    {
        MPI_Isend(&sendbuf, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &requests[i]);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    
    for (i = 0; i < size; ++i)
    {
        MPI_Irecv(&recvbuf[i], 1, MPI_INT, i, tag, MPI_COMM_WORLD, &requests[i]);
    }
    
    MPI_Waitall(size, requests, status);
    
    for (i = 0; i < size; ++i)
        cout << "Rank is " << rank << " got " << recvbuf[i] << endl;
    
    MPI_Finalize();
}