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
    int row[n];

    int send_row[n];
    int recv_col[n];

    cout << "rank " << rank << " sent ";
    for (int i = 0; i < n; ++i)
    {
        send_row[i] = row[i] = rank;
        cout << send_row[i] << ' ';
    }
    cout << endl;
    
    cout << "rank " << rank << " got ";
    for (int i = 0; i < n; ++i)
    {
        MPI_Allgather(&send_row[i], size, MPI_INT, recv_col, size, MPI_INT, MPI_COMM_WORLD);
        int sum = 0;
        for (int j = 0; j < n; ++j)
            sum += recv_col[j] * row[j];
        cout << sum << ' ';
    }
    cout << endl;
    
    MPI_Finalize();
}