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
    MPI_Status status;
    MPI_Request request;
    int *sendbuf = new int[size];
    int *recvbuf = new int[size];

    double starttime, endtime;
    starttime = MPI_Wtime();

    if (rank == 0 )
    {
        for (int i = 0; i < n; ++i)
            MPI_Isend(sendbuf, size, MPI_INT, i, tag, MPI_COMM_WORLD, &request);
    }

    MPI_Irecv(recvbuf, size, MPI_INT, 0, tag, MPI_COMM_WORLD, &request);
    MPI_Waitall(1, &request, &status);
    endtime = MPI_Wtime();
    cout << endtime-starttime 
        << " seconds for my bcast in " << rank << " process\n";



    starttime = MPI_Wtime();
    
    MPI_Bcast(sendbuf, size, MPI_INT, 0, MPI_COMM_WORLD);

    
    endtime = MPI_Wtime();
    cout << endtime-starttime 
        << " seconds for MPI_Bcast in " << rank << " process\n";
    delete []sendbuf;
    delete []recvbuf;
    MPI_Finalize();
}