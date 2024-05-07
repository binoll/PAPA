#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_MSG_SIZE 100000000

using namespace std;

int main(int argc, char *argv[])
{
	int rank, opt, size;

	while ((opt = getopt(argc, argv, "s:")) != -1) 
    {
        switch (opt) 
		{
        case 's':
            size = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Использование: %s [-n process_id]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    } 

	double starttime, endtime;
	++size;
	char* msg = new char[size];
	for (int i = 0; i < size; ++i) msg[i] = '!';
	msg[size - 1] = '\0';
	
	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Request req;
	
	char* sendbuff = new char[size];
	char* recvbuff = new char[size];
	
	// Получаем ранг текущего процессора
	if (MPI_Comm_rank( MPI_COMM_WORLD, &rank)) perror("Err at Comm_rank :( \n");	
	
	starttime = MPI_Wtime();
	if (rank == 0)
	{
		sendbuff = msg;
		MPI_Send(sendbuff, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD);
	}
	if (rank == 1)
	{
		MPI_Recv(recvbuff, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);
		//printf("\n Message from 0 rank process - %s\n", recvbuff);
		endtime = MPI_Wtime();
		printf("\nThat took %f seconds for block\n",endtime-starttime);
	}

	if (rank == 0)
	{
		sendbuff = msg;
		MPI_Isend(sendbuff, size, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &req);
	}
	if (rank == 1)
	{
		MPI_Irecv(recvbuff, size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &req);
		MPI_Wait(&req, &status);
		//printf("\n Message from 0 rank process - %s\n", recvbuff);
		endtime = MPI_Wtime();
		printf("\nThat took %f seconds for unblock\n",endtime-starttime);
	}
	
	MPI_Finalize();
}