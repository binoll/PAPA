#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int rank, namelen;
    char proc_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Request request;
    MPI_Status status;

	char* sendbuf = "hello world Rank:";
	char* recvbuf = malloc(MAX_BUF_SIZE * sizeof(char));

    int size = strlen(sendbuf);
    
    if (rank == 0)
    {
		MPI_Isend(sendbuf, size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD, &request);
    }
    else
    {
        MPI_Irecv(recvbuf, size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);

        printf("%s %d\n", recvbuf, rank);

        MPI_Isend(sendbuf, size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD, &request);
        MPI_Isend(sendbuf, size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD, &request);
    }

    MPI_Finalize();
    
}