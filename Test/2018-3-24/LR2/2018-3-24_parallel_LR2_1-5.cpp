#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_MSG_SIZE 1024

char numarr[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

void SenAllGather(char* sendbuff, char** recvbuff, int rank, int msg_size)
{	
    int allnum;
    MPI_Comm_size(MPI_COMM_WORLD, &allnum);

    MPI_Status status;
    MPI_Request request;
	
	for (int i = 0; i < allnum; ++i)
	{
		MPI_Isend(sendbuff, msg_size, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
		MPI_Irecv(recvbuff[i], msg_size, MPI_CHAR, i, 1, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
	}
}

int main(int argc, char *argv[])
{
	int rank, opt, allnum, size;

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
	
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &allnum);
	
    double starttime, endtime;
	
	int msg_size = size;
	char* msg = new char[msg_size];
	for (int i = 0; i < size; ++i) msg[i] = numarr[rank];
	//msg[msg_size - 1] = '\0';

    char* sendbuff = new char[msg_size];
	char** recvbuff;

	sendbuff = msg;
	//printf("\n Message in %d rank process to send: %.*s\n", rank, msg_size, sendbuff);
	
	recvbuff = new char*[allnum];
	for (int i = 0; i < allnum; ++i) recvbuff[i] = new char[msg_size];
	
	starttime = MPI_Wtime();
    SenAllGather(sendbuff, recvbuff, rank, msg_size);
	endtime = MPI_Wtime();
	if(rank == 0) printf("\nThat took %f seconds for SENfunction\n",endtime-starttime);
	
	for (int i = 0; i < allnum; ++i)
		{
			//printf("\n Messages in %d rank process: recieved - %.*s\n", rank, msg_size, recvbuff[i]);
		}
		
	MPI_Barrier(MPI_COMM_WORLD);
	
	char* recv = new char[allnum*msg_size];
	starttime = MPI_Wtime();
    MPI_Allgather(msg, msg_size, MPI_CHAR, recv, msg_size, MPI_CHAR, MPI_COMM_WORLD);
	//printf("\n !!!Messages in %d rank process: recieved - %.*s\n", rank, allnum*msg_size, recv);
	endtime = MPI_Wtime();
	if(rank == 0) printf("\nThat took %f seconds for MPIfunction\n",endtime-starttime);
	
    MPI_Finalize();
}
