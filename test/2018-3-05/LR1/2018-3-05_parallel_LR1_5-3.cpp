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

	MPI_Status status;
	MPI_Request request;

	char* send = new char;
	*send = char(int('a') + myrank);

	char* rec = new char;

	for (int i = 0; i < process_count; i++)
	{
		

		if (myrank == i) continue;
		else 
		{
			MPI_Isend(send, 1, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);

			MPI_Recv(rec, 1, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);

			printf("From process rank with %d to process with rank %d. Message: %c\n",
				i, myrank, *rec);
		}
	}

	MPI_Finalize();

	return 0;

}
