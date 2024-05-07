#include <stdio.h>
#include "mpi.h"

#define MAX_BUFF_SIZE 1024
#define MESSAGE_SIZE 512

int main(int argc, char* argv[])
{
    int n, tag, rank;
    double starttime, endtime;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    char* message = malloc(MAX_BUFF_SIZE);
    char* recvbuf = malloc(MAX_BUFF_SIZE);
    message = "Glushak";
    MPI_Init(&argc, &argv);
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n);

    if (rank == 0)
    {
        MPI_Send(message, 1024, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        printf("Send message in rank %d: %s\n", rank, message);
    }
    else
    {   
        if (rank + 1 < n)
        {
            MPI_Recv(recvbuf, 1024, MPI_CHAR, rank - 1, 0, MPI_COMM_WORLD, &status);
            printf("Received message in rank %d: %s\n", rank, recvbuf);
            MPI_Send(recvbuf, 1024, MPI_CHAR, rank + 1, 0, MPI_COMM_WORLD);
            printf("Send message in rank %d: %s\n", rank + 1, recvbuf);
        }
        else
        {
            MPI_Recv(recvbuf, 1024, MPI_CHAR, rank - 1, 0, MPI_COMM_WORLD, &status);
            printf("Received message in rank %d: %s\n", rank, recvbuf);
            MPI_Send(recvbuf, 1024, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            printf("Send message in rank %d: %s\n", 0, recvbuf);
        }
    }

    MPI_Finalize();
    return 0;
}