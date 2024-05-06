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

    int size = 1;
    int sendbuf[n];
    int recvbuf[n];

    cout << "rank " << rank << " sent ";
    for (int i = 0; i < n; ++i)
    {
        sendbuf[i] = 10*i + rank;
        cout << sendbuf[i] << ' ';
    }
    cout << endl;
    
    MPI_Scan (sendbuf, recvbuf, n, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    cout << "rank " << rank << " got ";
    for (int i = 0; i < n; ++i)
        cout << recvbuf[i] << ' ';
    cout << endl;

    MPI_Finalize();
}