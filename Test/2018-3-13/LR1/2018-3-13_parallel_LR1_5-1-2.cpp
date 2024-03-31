#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"
#include <time.h>

#define SIZE_BUF 1024

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, procs, tag;
    char proc_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Status status;

    int sendbuf[] = { rank };
    int* recvbuf = malloc(SIZE_BUF*sizeof(int));
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

    MPI_Sendrecv(sendbuf, size, MPI_INT, dest, tag, recvbuf, SIZE_BUF*sizeof(int), MPI_INT,
        source, tag, MPI_COMM_WORLD, &status);
    printf("Sendrecv to rank %d: %d\n", rank, recvbuf[0]);

    MPI_Finalize();
}
