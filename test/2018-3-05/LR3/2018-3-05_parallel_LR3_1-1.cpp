#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "windows.h"


int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);

	int myrank, namelen, process_count = 0;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
 
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	MPI_Get_processor_name(processor_name, &namelen);

	int* sendbuf = new int[process_count];
	for (int i = 0; i < process_count; i++) sendbuf[i] = myrank * process_count + i;

	int* recvbuf = new int[process_count];

	MPI_Allreduce(sendbuf, recvbuf, process_count, MPI_INT, MPI_PROD, MPI_COMM_WORLD);
	for (int i = 0; i < process_count; i++) printf("Process with rank %d send process with rank %d message %d.\n", myrank, i, sendbuf[i]);

	for (int i = 0; i < process_count; i++) printf("From process with rank %d process with rank %d get message %d.\n", i, myrank, recvbuf[i]);

	MPI_Finalize();

	return 0;

}
