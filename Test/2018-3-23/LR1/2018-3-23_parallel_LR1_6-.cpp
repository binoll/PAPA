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
    int size = 1;
    MPI_Status *status = new MPI_Status[n];
    MPI_Request *requests = new MPI_Request[n];
    int send_vector[n];
    int recv_vector[n];
    
    int i;
    cout << "rank " << rank << " send ";
    for (i = 0; i < n; ++i)
    {
        send_vector[i] = rank;
        cout << send_vector[i];
    }
    cout << endl;
    
    for (i = 0; i < n; ++i)
    {
        MPI_Isend(&send_vector[i], size, MPI_INT, i, tag, MPI_COMM_WORLD, &requests[i]);
        MPI_Irecv(&recv_vector[i], size, MPI_INT, i, tag, MPI_COMM_WORLD, &requests[i]);
    }
    
    MPI_Waitall(n, requests, status);
    
    cout << "rank " << rank << " got ";
    for (i = 0; i < n; ++i)
        cout << recv_vector[i];
    cout << endl;
    
    delete[] status;
    delete[] requests;
    MPI_Finalize();
}