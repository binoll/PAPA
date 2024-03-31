#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

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
    
    int tag = 0;
    MPI_Status *status = new MPI_Status[n];
    MPI_Request *requests = new MPI_Request[n];
    int *send_vector = new int[size];
    int *recv_vector = new int[size];

    double starttime, endtime;
    starttime = MPI_Wtime();

    for (int i = 0; i < n; ++i)
    {
        MPI_Isend(send_vector, size, MPI_INT, i, tag, MPI_COMM_WORLD, &requests[i]);
        MPI_Irecv(recv_vector, size, MPI_INT, i, tag, MPI_COMM_WORLD, &requests[i]);
    }
    MPI_Waitall(n, requests, status);
    
    endtime = MPI_Wtime();
    cout << endtime-starttime 
        << " seconds for my barrier in " << rank << " process\n";

    starttime = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    endtime = MPI_Wtime();
    cout << endtime-starttime 
        << " seconds for MPI_Barrier in " << rank << " process\n";
    delete[] status;
    delete[] requests;
    delete[] send_vector;
    delete[] recv_vector;
    MPI_Finalize();
}