#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"
#include <time.h>

#define MAX_BUF_SIZE 1024

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int rank, procs;
    int tag = 1;
    char proc_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    MPI_Status status;

    int sendbuf[] = { rank };
	//char* recvbuf = malloc(MAX_BUF_SIZE * sizeof(int));
    int* recvbuf = malloc(procs * sizeof(int));

    int size = 1;

    int dest = rank + 1;
    int source = rank - 1;
    
    if (rank == 0)
    {
        source = procs - 1;
    }
    else if (rank == (procs - 1))
    {
        dest = 0;
    }

    MPI_Sendrecv(sendbuf, size, MPI_INT,
        dest, tag,
        recvbuf, procs * sizeof(int), MPI_INT,
        source, tag,
        MPI_COMM_WORLD, &status);
    //MPI_Barrier(MPI_COMM_WORLD);
    printf("Message from RANK %d to RANK %d -> %d\n", source, rank, recvbuf[0]);

    MPI_Finalize();
}
