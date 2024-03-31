#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int rank, procs, namelen;
    char proc_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    MPI_Request request;
    MPI_Status status;

	int* recv = (int *)malloc(10 * sizeof(int));

	int send[] = { rank };
	int i = 0;

	while (i < procs)
	{
		if (i == rank)
		{
			i++;
			continue;
		}

		MPI_Isend(send, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &request);

		MPI_Recv(recv, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);

		printf("rank=%d from rank=%d, recv='%d'\n", rank, i, recv[0]);
		i++;
	}
	MPI_Finalize();
	return 0;
}
