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

    MPI_Status status[n];
    MPI_Request requests[n];
    int *sendbuf = new int[size];
    int **recvbuf = new int*[n];
    for (int i = 0; i < n; ++i)
        recvbuf[i] = new int[size];

  
    double starttime, endtime;
  
    starttime = MPI_Wtime();

    MPI_Isend(sendbuf, size, MPI_INT, 0, tag, MPI_COMM_WORLD, &requests[0]);

    if (rank == 0 )
    {
        for (int i = 0; i < n; ++i)
            MPI_Irecv(recvbuf[i], size, MPI_INT, i, tag, MPI_COMM_WORLD, &requests[i]);
        MPI_Waitall(n, requests, status);

    }

    endtime = MPI_Wtime();
    cout << endtime-starttime 
        << " seconds for my gather in " << rank << " process\n";
    
   
    int *recv = new int[size *n];
    starttime = MPI_Wtime();
    MPI_Gather(sendbuf, size, MPI_INT, recv, size, MPI_INT, 0, MPI_COMM_WORLD); 

    endtime = MPI_Wtime();
    cout << endtime-starttime 
        << " seconds for my MPI_Gather in " << rank << " process\n";
    

    delete [] sendbuf;
    for (int i = 0; i < n; ++i)
         delete [] recvbuf[i];
    delete [] recvbuf;
    delete [] recv;
    MPI_Finalize();
}