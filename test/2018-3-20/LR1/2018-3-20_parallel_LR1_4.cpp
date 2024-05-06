#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int rank, size, namelen;
    char proc_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double start, end;
    MPI_Request request;
    MPI_Status status;

	char* sendbuf0 = "BLOK hello world Rank: 0";
	char* sendbuf1 = "BLOK hello world Rank: 1";
	char* sendbuf2 = "UNBLOK hello world Rank: 0";
	char* sendbuf3 = "UNBLOK hello world Rank: 3";

	char* recvbuf = malloc(MAX_BUF_SIZE * sizeof(char));

    start = MPI_Wtime();
    
    if (rank == 0)
    {
		int size = strlen(sendbuf0);
		MPI_Send(sendbuf0, size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD);
		MPI_Recv(recvbuf, size, MPI_CHAR, rank + 1, 1, MPI_COMM_WORLD, &status);
	
		end = MPI_Wtime();
	
		printf("RecvBuf Rank 0:%s, Time:%f\n", recvbuf, (end - start) / 2);
    }
    else if (rank == 1)
    {
		int size = strlen(sendbuf1);
		char* recvbuf = malloc(MAX_BUF_SIZE * sizeof(char));
		MPI_Recv(recvbuf, size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD, &status);
		MPI_Send(sendbuf1, size, MPI_CHAR, rank - 1, 1, MPI_COMM_WORLD);
	
		end = MPI_Wtime();
	
		printf("RecvBuf Rank 1:%s, Time:%f\n", recvbuf, (end - start) / 2);
    }
    if (rank == 0)
    {
		int size = strlen(sendbuf2);
		MPI_Isend(sendbuf2, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &request);
		//MPI_Irecv(recvbuf, MAX_BUF_SIZE * sizeof(char), MPI_CHAR, 1, 1, MPI_COMM_WORLD, &request[1]);
	
		//printf("RANK 2 %s\n", recvbuf);
    }
    else if (rank == 1)
    {
		int size = strlen(sendbuf3);
	
		//MPI_Isend(sendbuf3, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &request[0]);
		MPI_Irecv(recvbuf, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &request);
    }

	if (rank == 1)
	{
		MPI_Wait(&request, &status);

		end = MPI_Wtime();

		printf("RecvBuf Rank 1:%s, Time:%f\n", recvbuf, end - start);
	}
    
    MPI_Finalize();
    
}