#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

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
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int tag = 0;
    MPI_Status status;

    double starttime, endtime;
    
    int * buf = new int[size];
    
    for (int i=0; i < size; ++i)
        buf[i]=rank;
    
    starttime = MPI_Wtime();
    
    if (rank == 0)
        MPI_Send(buf, size, MPI_INT, 1, tag, MPI_COMM_WORLD);
    else if (rank == 1)
        MPI_Recv(buf, size, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
    
    endtime = MPI_Wtime();
    printf("Massage (%d ints) took %f seconds for %d blocked process\n", size, endtime-starttime, rank);
    
    
    for (int i=0; i < size; ++i)
        printf("%d ", buf[i]);
    printf("\n");
    
    MPI_Request requests[1];
    
    starttime = MPI_Wtime();
    
    if (rank == 0)
        MPI_Isend(buf, size, MPI_INT, 1, tag, MPI_COMM_WORLD, &requests[0]);
    else if (rank == 1)
        MPI_Irecv(buf, size, MPI_INT, 0, tag, MPI_COMM_WORLD, &requests[0]);
    
    MPI_Waitall(1, requests, &status);
    endtime = MPI_Wtime();
    printf("Massage (%d ints) took %f seconds for %d unblocked process\n", size, endtime-starttime, rank);
    
    
    
    for (int i=0; i < size; ++i)
        printf("%d ", buf[i]);
    printf("\n");
    
    delete[] buf;
    MPI_Finalize();
}