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
    MPI_Status * status = new MPI_Status[n];
    MPI_Request *requests = new MPI_Request[n];

    int **sendbuf = new int*[n];
    for (int i = 0; i < n; ++i)
        sendbuf[i] = new int[size];

    int **recvbuf = new int*[n];
    for (int i = 0; i < n; ++i)
        recvbuf[i] = new int[size];
    
    double starttime, endtime;    
    starttime = MPI_Wtime();

    for (int i = 0; i < n; ++i)
        MPI_Isend(sendbuf[i], size, MPI_INT, i, tag, MPI_COMM_WORLD, &requests[i]);
    
    for (int i = 0; i < n; ++i)
        MPI_Irecv(recvbuf[i], size, MPI_INT, i, tag, MPI_COMM_WORLD, &requests[i]);
    
    MPI_Waitall(n, requests, status);

    endtime = MPI_Wtime();
    cout << endtime-starttime 
        << " seconds for my alltoall in " << rank << " process\n";
    
    int *send = new int[size * n];
    int *recv = new int[size * n];
    starttime = MPI_Wtime();

    MPI_Alltoall(send, size, MPI_INT, recv, size, MPI_INT, MPI_COMM_WORLD);

    endtime = MPI_Wtime();
    cout << endtime-starttime 
        << " seconds for my MPI_Alltoall in " << rank << " process\n";



    for (int i = 0; i < n; ++i)
         delete [] sendbuf[i];
    for (int i = 0; i < n; ++i)
         delete [] recvbuf[i];
    delete [] status;
    delete [] requests;
    MPI_Finalize();
}