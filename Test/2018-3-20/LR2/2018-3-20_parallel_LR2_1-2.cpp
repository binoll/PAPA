#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include "mpi.h"

#define MAX_HOST_NAME 1024
#define MAX_BUF_SIZE 1024

int getSizeData(int argc, char** argv, int* sizeData) 
{
    int code, count, err = MPI_SUCCESS;

    code = getopt(argc, argv, "n:");
    if (code == 'n') {
        count = sscanf(optarg, "%d", sizeData);
    }
    return err;
}

void My_Bcast(int sendbuf[], int* recvbuf, int rank, int procs, int number, MPI_Status status, MPI_Request request)
{
    if (rank == 0)
    {
        for (int i = 1; i < procs; i++)
        {
            MPI_Isend(sendbuf, number, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
        }
    }
    else
    {
        MPI_Irecv(recvbuf, number, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);

        for (int i = 0; i < number; i++)
        {
            printf("Rank -> %d Recv Buffer -> %d ", rank, sendbuf[i]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int number, err;
    err = getSizeData(argc, argv, &number);
    
    int rank, procs;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    MPI_Status status;
    MPI_Request request;
    int* recvbuf = new int(sizeof(int) * number);


    double start, end;

    int sendbuf[number];

    for (int i = 0; i < number; i++)
    {
        sendbuf[i] = i;
    }
    if (rank == 0)
    {
        start = MPI_Wtime();
        My_Bcast(sendbuf, recvbuf, rank, procs, number, status, request);
        end = MPI_Wtime();

        sleep(1);

        printf("My Bcast working -> %lf\n", (end - start));
    }

    start = MPI_Wtime();
    MPI_Bcast(sendbuf, number, MPI_INT, 0, MPI_COMM_WORLD);
    end = MPI_Wtime();

    sleep(1);

    if (rank == 0)
    {
        printf("MPI_Bcast working -> %lf\n", (end - start));
    }


    MPI_Finalize();
    
}