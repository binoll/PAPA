#include <stdio.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define MAX_BUF_SIZE 1024
int main(int argc, char *argv[])
{
    int n, rank, namelen, opt, size_sm;
    char *msg_1;
    char *msg_2;
    char *recvbuf[MAX_BUF_SIZE];
    int size;
    int* testbuf;
    int* testbuf_recv;
    if (argc <= 1)
    {
	fprintf(stderr, "Invalid arguments. Use: [-s message_1 -f message_2]\n");
	exit(EXIT_FAILURE);
    }
    while ((opt = getopt(argc, argv, "s:f:n:")) != -1)
    {
	switch (opt)
	{
	    case 's':
		msg_1 = optarg;
		break;
	    case 'f':
		msg_2 = optarg;
		break;
        case 'n':
        size = atoi(optarg);
        break;
	    default:
		fprintf(stderr, "Invalid arguments. Use: [-s message_1 -f message_2]\n");
		exit(EXIT_FAILURE);
	}
    }
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Request request[2];
    MPI_Status status[2];
    double starttime, endtime;
 //   starttime = MPI_Wtime();
 //   if (rank == 0)
 //   {
	//MPI_Isend(msg_1, strlen(msg_1), MPI_CHAR, 1, 22, MPI_COMM_WORLD, &request[0]);
	//MPI_Irecv(recvbuf, MAX_BUF_SIZE, MPI_CHAR, 1, 22, MPI_COMM_WORLD, &request[1]);
	////printf("message on rank 0: %s\n", recvbuf);
 //   }
 //   else if (rank == 1)
 //   {
	////MPI_Isend(msg_2, strlen(msg_2), MPI_CHAR, 0, 22, MPI_COMM_WORLD, &request[0]);
	//MPI_Irecv(recvbuf, MAX_BUF_SIZE, MPI_CHAR, 0, 22, MPI_COMM_WORLD, &request[0]);
	//MPI_Isend(msg_2, strlen(msg_2), MPI_CHAR, 0, 22, MPI_COMM_WORLD, &request[1]);
	////printf("message on rank 1: %s\n", recvbuf);
 //   }
 //   MPI_Waitall(2, request, status);
 //   printf("rank: %d\tmessage:%s\n", rank, recvbuf);
 //   endtime = MPI_Wtime();

	starttime = MPI_Wtime();
	if (rank == 0)
	{
		testbuf = (int*)malloc(size * sizeof(int));
		for (int i = 0; i < size; i++)
		{
			testbuf[i] = i;
		}
		MPI_Isend(testbuf, size, MPI_INT, 1, 22, MPI_COMM_WORLD, &request[0]);
	}
	else if (rank == 1)
	{
		testbuf_recv = (int*)malloc(size * sizeof(int));
		MPI_Irecv(testbuf_recv, size, MPI_INT, 0, 22, MPI_COMM_WORLD, &request[0]);
	}
	MPI_Waitall(1, request, status);
	endtime = MPI_Wtime();
	if (rank == 1)
	{
		printf("That took in rank %d: %f seconds\n", rank, endtime - starttime);
	}
    MPI_Finalize();
}