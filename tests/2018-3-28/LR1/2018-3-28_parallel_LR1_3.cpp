#include <stdio.h>
#include "mpi.h"
#include <unistd.h>

#define MAX_BUFF_SIZE 1024
#define MESSAGE_SIZE 512

int main(int argc, char* argv[])
{
    int opt, n, tag, rank, bufsize;
    double starttime, endtime;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    char* recvbuf = malloc(MAX_BUFF_SIZE);
    while ((opt = getopt(argc, argv, "s:")) != -1)
    {
        switch (opt)
        {
        case 't':
            bufsize = atoi(optarg);
            break;
        }
    }

    char* message = malloc(bufsize);
    int i;
    for (i = 0; i < bufsize; ++i)
    {
        message[i] = 'a';
    }

    MPI_Init(&argc, &argv);
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    starttime = MPI_Wtime();
    if (rank == 0)
    {
        MPI_Send(message, bufsize, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        //printf("Send message in rank %d: %s\n", rank, message);

    }
    else if (rank == 1)
    {
        MPI_Recv(recvbuf, bufsize, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
        //printf("Received message in rank %d: %s\n", rank, recvbuf);
    }
    else {}
    endtime = MPI_Wtime();
    MPI_Finalize();
    printf("That took %f seconds\n", endtime - starttime);
    return 0;
}