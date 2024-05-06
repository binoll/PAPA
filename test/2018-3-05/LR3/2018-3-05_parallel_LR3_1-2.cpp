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

	int* count = new int[argc - 1];
	int sendbuf_size = 0;

	for (int i = 1; i < argc; i++)
	{
		count[i - 1] = atoi(argv[i]);
		sendbuf_size += atoi(argv[i]);
	}

	int* sendbuf = new int[sendbuf_size];
	for (int i = 0; i < sendbuf_size; i++) sendbuf[i] = myrank * sendbuf_size + i;
	for (int i = 0; i < sendbuf_size; i++) printf("Process with rank %d sendbuf[%d] is: %d\n", myrank, i, sendbuf[i]);

	int* recvbuf = new int[count[myrank]];

	MPI_Reduce_scatter(sendbuf, recvbuf, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	for (int i = 0; i < count[myrank]; i++) printf("Process with rank %d RECVBUF[%d] is: %d\n", myrank, i, recvbuf[i]);

	MPI_Finalize();

	return 0;

}
