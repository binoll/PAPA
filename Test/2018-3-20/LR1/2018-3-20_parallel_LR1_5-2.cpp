#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int rank, procs;
    char proc_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    MPI_Request request;
    MPI_Status status;
    
    if (rank == 0)
    {

        char* recvbuf = malloc(MAX_BUF_SIZE * sizeof(char));

        int i = 1;

        while (i < procs)
        {
            MPI_Recv(recvbuf, MAX_BUF_SIZE * sizeof(char), MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);
            printf("Message from Proc %d to MASTER -> %s\n", i, recvbuf);
            i++;
        }
    }
    else
    {
        char* sendbuf = "hello world";
        int size = strlen(sendbuf);
        MPI_Send(sendbuf, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    
}