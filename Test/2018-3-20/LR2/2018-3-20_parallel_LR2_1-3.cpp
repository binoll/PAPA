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

void My_Gather(int** sendbuf, int* recvbuf,int number, int rank, int procs, MPI_Status status, MPI_Request request)
{
    if (rank == 0)
    {
        for (int i = 0; i < procs; i++)
        {
            if (i != rank)
            {
                MPI_Irecv(recvbuf, number, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
                MPI_Wait(&request, &status);

                printf("Rank -> %d Recv Buffer -> ", rank);

                for (int i = 0; i < number; i++)
                {
                    printf(" %d", recvbuf[i]);
                }
                printf("\n");
            }
        }
    }
    else
    {
        for (int i = 0; i < procs; i++)
        {
            MPI_Isend(sendbuf[i], number, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        }
    }
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int number, err;
    err = getSizeData(argc, argv, &number);

    int rank, procs;
    char proc_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    MPI_Status status;
    MPI_Request request;
    int* recvbuf1 = new int[number];
    int** recvbuf2 = new int* [procs];

    int** sendbuf = new int* [procs];

    for (int i = 0; i < procs; i++)
    {
        sendbuf[i] = new int[number];
    }

    for (int i = 0; i < procs; i++)
    {
        recvbuf2[i] = new int[number];
    }


    for (int i = 0; i < procs; i++)
    {
        for (int j = 0; j < number; j++)
        {
            sendbuf[i][j] = rank;
        }
    }

    double start, end;

    start = MPI_Wtime();
    My_Gather(sendbuf, recvbuf1, number, rank, procs, status, request);
    end = MPI_Wtime();

    sleep(1);

    if (rank == 0)
    {
        printf("My Gather working -> %lf\n", rank, (end - start));
        printf("\n");
    }


    start = MPI_Wtime();
    MPI_Gather(sendbuf, number, MPI_INT, recvbuf2, number, MPI_INT, 0, MPI_COMM_WORLD);
    end = MPI_Wtime();

    if (rank == 0)
    {
        printf("MPI_Gather working -> %lf\n", (end - start));
        sleep(1);
    }


    MPI_Finalize();

}