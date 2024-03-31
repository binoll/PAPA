#include <stdio.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define MAX_BUF_SIZE 1024
int main(int argc, char *argv[])
{
    int n, rank, opt;
    char *msg_1;
    char *msg_2;
    char *recvbuf[MAX_BUF_SIZE];
    int size;
    int* testbuf;
    int* testbuf_recv;
    if (argc <= 1)
    {
	fprintf(stderr, "Invalid argument. Use: [-s message 1 -f message 2]\n");
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
		fprintf(stderr, "Invalid argument. Use: %s [-s message 1 -f message 2]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
    }
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status status;
    double starttime, endtime;
    double start, end;

    //starttime = MPI_Wtime();
    //if (rank == 0)
    //{
	   // MPI_Send(msg_1, strlen(msg_1), MPI_CHAR, 1, 22, MPI_COMM_WORLD);
	   // MPI_Recv(recvbuf, MAX_BUF_SIZE, MPI_CHAR, 1, 22, MPI_COMM_WORLD, &status);
	   // printf("message on rank 0: %s\n", recvbuf);
    //}
    //else if (rank == 1)
    //{
	   // MPI_Recv(recvbuf, MAX_BUF_SIZE, MPI_CHAR, 0, 22, MPI_COMM_WORLD, &status);
	   // printf("message on rank 1: %s\n", recvbuf);
	   // MPI_Send(msg_2, strlen(msg_2), MPI_CHAR, 0, 22, MPI_COMM_WORLD);
    //}
    //endtime = MPI_Wtime();

    start = MPI_Wtime();
    if (rank == 2)
    {
        testbuf = (int*)malloc(size * sizeof(int));
        for (int i = 0; i < size; i++)
        {
            testbuf[i] = i;
        }
        MPI_Send(testbuf, size, MPI_INT, 3, 22, MPI_COMM_WORLD);
    }
    else if (rank == 3)
    {
        testbuf_recv = (int*)malloc(size * sizeof(int));
        MPI_Recv(testbuf_recv, size, MPI_INT, 2, 22, MPI_COMM_WORLD, &status);
    }
    end = MPI_Wtime();
    if (rank == 3)
    {
        printf("test took %f seconds\n", end - start);
    }
    MPI_Finalize();
}