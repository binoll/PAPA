#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#define MAX_MESSAGE_LEN 512

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);

	int myrank, namelen, process_count;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	MPI_Get_processor_name(processor_name, &namelen);

	double starttime_locked, endtime_locked, starttime_unlocked, endtime_unlocked;
	starttime_locked = MPI_Wtime();
	starttime_unlocked = MPI_Wtime();

	char sendbuf_locked[] = "This is process with rank 0!";
	char sendbuf_unlocked[] = "This is process with rank 2!";

	MPI_Status status;
	MPI_Request request;

	char* recvbuf_locked = new char[MAX_MESSAGE_LEN];
	char* recvbuf_unlocked = new char[MAX_MESSAGE_LEN];

	if (myrank == 0)
	{
		//char sendbuf_locked[] = "This is process with rank 0!";

		MPI_Send(&sendbuf_locked, strlen(sendbuf_locked), MPI_INT, 1, 1, MPI_COMM_WORLD);

		//printf("Process rank: %d, That took %f seconds with locked exchange\n", myrank, endtime_locked - starttime_locked);

	}
	else if (myrank == 1)
	{

		MPI_Recv(recvbuf_locked, strlen(sendbuf_locked), MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		endtime_locked = MPI_Wtime();

		printf("The process with rank %d get message: %s", myrank, recvbuf_locked);
		/*for (int i = 0; i < MAX_MESSAGE_LEN; i++)
		{
			printf("%c", recvbuf_locked[i]);
		}*/

		printf("\n");
		printf("Process rank: %d, That took %f seconds with locked exchange\n", myrank, endtime_locked - starttime_locked);

		delete recvbuf_locked;
	}
	else if (myrank == 2)
	{
		//char sendbuf_unlocked[] = "This is process with rank 2!";

		MPI_Isend(&sendbuf_unlocked, strlen(sendbuf_unlocked), MPI_INT, 3, 2, MPI_COMM_WORLD, &request);

		//printf("Process rank: %d, That took %f seconds with unlocked exchange\n", myrank, endtime_unlocked - starttime_unlocked);
		
	}
	else if (myrank == 3)
	{

		MPI_Irecv(recvbuf_unlocked, strlen(sendbuf_unlocked), MPI_INT, 2, 2, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
		endtime_unlocked = MPI_Wtime();

		printf("The process with rank %d get message: %s", myrank, recvbuf_unlocked);
		/*for (int i = 0; i < MAX_MESSAGE_LEN; i++)
		{
			printf("%c", recvbuf_unlocked[i]);
		}*/

		printf("\n");
		printf("Process rank: %d, That took %f seconds with unlocked exchange\n", myrank, endtime_unlocked - starttime_unlocked);

		delete recvbuf_unlocked;
	}

	MPI_Finalize();

}
