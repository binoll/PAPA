#include "mpi.h"
#include <stdio.h>
#include <math.h>
#define MAX_MESSAGE_LEN 30

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);

	int myrank, namelen, process_count;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	MPI_Get_processor_name(processor_name, &namelen);

	int sendbuf[process_count], recvbuf[process_count];

	MPI_Status status[process_count - 1];
	MPI_Request request[process_count - 1];

	for (int i = 0; i < process_count; i++) sendbuf[i] = myrank * process_count + i;

	for (int i = 0, req = 0; i < process_count; i++)
	{
		if (i == myrank)
			continue;
		MPI_Isend(&sendbuf[i], 1, MPI_INT, i, 1, MPI_COMM_WORLD, &request[req]);
		req++;
	}

	for (int i = 0, req = 0; i < process_count; i++)
	{
		if (i == myrank)
			recvbuf[i] = sendbuf[i];
		else
		{
			MPI_Irecv(&recvbuf[i], 1, MPI_INT, i, 1, MPI_COMM_WORLD, &request[req]);
			req++;
		}
		
	}

	MPI_Waitall(process_count - 1, request, status);

	printf("Process with rank %d , send [ ", myrank);
	for (int i = 0; i < process_count; i++)
	{
		printf("%d ", sendbuf[i]);
	}
	printf("]\n");

	printf("Process with rank %d , recv [ ", myrank);
	for (int i = 0; i < process_count; i++)
	{
		printf("%d ", recvbuf[i]);
	}
	printf("]\n");

	MPI_Finalize();

	return 0;

}
