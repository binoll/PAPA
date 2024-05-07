#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <malloc.h>
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
    MPI_Request request;
    MPI_Status status;

    int sendbuf[] = { rank };
    int* recvbuf = (int*)malloc(10* sizeof(int));
    int i = 0;

    while (i < procs) {
        if (i == rank) {
            i++;
            continue;
        }
        MPI_Isend(sendbuf, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &request);
        MPI_Recv(recvbuf, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
        printf("Recv rank=%d send rank=%d, messege='%d'\n", rank, i, recvbuf[0]);
        i++;
    }
    
    MPI_Finalize();
}
