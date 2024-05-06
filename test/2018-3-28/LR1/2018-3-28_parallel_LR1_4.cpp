#include <stdio.h>
#include "mpi.h"

#define MIN_BUFF_SIZE 2
#define MAX_BUFF_SIZE 2000000
#define MESSAGE_SIZE 512

int main(int argc, char* argv[])
{
    int n, tag, rank, size;
    double starttime, endtime, starttimeBlock, endtimeBlock;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    char* message = malloc(MAX_BUFF_SIZE);
    char* recvbuf = malloc(MAX_BUFF_SIZE);
    MPI_Init(&argc, &argv);
    MPI_Status status, newStatus;
    MPI_Request request;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    for (size = 1000; size < 2000000; size = size * 2)
    {
        printf("Experiment for size = %d ", size);

        int i;
        for (i = 0; i < size; i++)
            message[i] = 'a';

        starttime = MPI_Wtime();
        if (rank == 0)
        {
            MPI_Isend(message, size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &request);
        }
        else if (rank == 1)
        {
            MPI_Irecv(recvbuf, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request);
        }
        MPI_Wait(&request, &newStatus);
        endtime = MPI_Wtime();

        starttimeBlock = MPI_Wtime();
        if (rank == 0)
        {
            MPI_Send(message, size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        }
        else if (rank == 1)
        {
            MPI_Recv(recvbuf, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
        }
        endtimeBlock = MPI_Wtime();

        if (rank == 0)
            printf("That unblock took %f seconds\n that block took %f seconds\n", endtime - starttime, endtimeBlock - starttimeBlock);
    }
    MPI_Finalize();

    return 0;
}