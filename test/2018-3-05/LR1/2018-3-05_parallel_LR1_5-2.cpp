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

	if (myrank == 0)
	{
		char* recvbuf = new char[MAX_MESSAGE_LEN];

		for (int i = 1; i < process_count; i++)
		{
			MPI_Recv(recvbuf, MAX_MESSAGE_LEN, MPI_CHAR, i, 1, MPI_COMM_WORLD, &status);

			printf("From process rank with %d to process with rank %d. Message: %s\n",
				i, myrank, recvbuf);
			
		}

		/*delete recvbuf;*/
	}
	else
	{
		char send = char(int('a') + myrank - 1);

		MPI_Send(&send, MAX_MESSAGE_LEN, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
	}

	MPI_Finalize();

}
