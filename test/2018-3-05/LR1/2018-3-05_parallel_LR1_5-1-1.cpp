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
		char* sendbuf = (char*)"MARKER";
		char* recvbuf = new char[MAX_MESSAGE_LEN];

		MPI_Send(sendbuf, MAX_MESSAGE_LEN, MPI_CHAR, myrank + 1, 1, MPI_COMM_WORLD);

		MPI_Recv(recvbuf, MAX_MESSAGE_LEN, MPI_CHAR, process_count - 1, 1, MPI_COMM_WORLD, &status);

		printf("Process rank: %d, The message: %s\n", myrank, recvbuf);

		/*delete sendbuf;
		delete recvbuf;*/
	}
	else
	{
		char* sendbuf = (char*)"MARKER";
		char* recvbuf = new char[MAX_MESSAGE_LEN];

		MPI_Recv(recvbuf, MAX_MESSAGE_LEN, MPI_CHAR, myrank - 1, 1, MPI_COMM_WORLD, &status);

		printf("Process rank: %d, The message: %s\n", myrank, recvbuf);

		if (myrank == process_count - 1)
		{
			MPI_Send(sendbuf, MAX_MESSAGE_LEN, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
		}
		else {
			MPI_Send(sendbuf, MAX_MESSAGE_LEN, MPI_CHAR, myrank + 1, 1, MPI_COMM_WORLD);
		}
		
		/*delete sendbuf;
		delete recvbuf;*/
	}

	MPI_Finalize();

}
