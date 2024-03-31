#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
	int procs, rank, namelen, func_res;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	if (MPI_Comm_size(MPI_COMM_WORLD, &procs))
		perror("Error at MPI_Comm_size!\n");
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_COMM_WORLD!\n");
	if (MPI_Get_processor_name(processor_name, &namelen))
		perror("Error at MPI_Get_processor_name!\n");

	
	printf("size=%d, rank=%d, name=%s\n", procs, rank, processor_name);
	
    
    
    MPI_Finalize();
    
}