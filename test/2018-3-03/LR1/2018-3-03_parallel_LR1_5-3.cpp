#include "mpi.h"
#include <stdio.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int n, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int tag = 0;
    MPI_Status * status = new MPI_Status[n];
    MPI_Request *requests = new MPI_Request[n];
    int max_count = 1;
    int sendbuf = rank;
    int *recvbuf = new int[n];
    
    cout << "my rank is " << rank << " and i send " << sendbuf << endl;
    
    int i = 0;
    for (i; i < n; ++i)
        MPI_Isend(&sendbuf, max_count, MPI_INT, i, tag, MPI_COMM_WORLD, &requests[i]);

    MPI_Barrier(MPI_COMM_WORLD);
    
    for (i = 0; i < n; ++i)
    {
        MPI_Irecv(&recvbuf[i], max_count, MPI_INT, i, tag, MPI_COMM_WORLD, &requests[i]);
    }
    
    MPI_Waitall(n, requests, status);
    
    for (i = 0; i < n; ++i)
        cout << "my rank is " << rank << " and i got " << recvbuf[i] << endl;
    
    delete [] status;
    delete [] recvbuf;
    delete [] requests;
    MPI_Finalize();
}
