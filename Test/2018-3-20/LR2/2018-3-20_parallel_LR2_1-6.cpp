#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <vector>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

using namespace std;

int getSizeData(int argc, char** argv, int* sizeData)
{
    int code, count, err = MPI_SUCCESS;

    code = getopt(argc, argv, "n:");
    if (code == 'n') {
        count = sscanf(optarg, "%d", sizeData);
    }
    return err;
}

void My_AlltoAll(int** sendbuf, int* recvbuf, int number, int rank, int procs, MPI_Status status, MPI_Request request)
{
    for (int i = 0; i < procs; i++)
    {
        MPI_Isend(sendbuf[i], number, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
        
    }

    for (int i = 0; i < procs; i++)
    {
        if (i == rank)
        {
            printf("Rank %d recv -> ", rank);

            for (int j = 0; j < number; j++)
            {
                printf("%d ", sendbuf[i][j]);
            }
            printf("\n");
            continue;
        }
        MPI_Irecv(recvbuf, number, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);

        printf("Rank %d recv -> ", rank);

        for (int i = 0; i < number; i++)
        {
            printf("%d ", recvbuf[i]);
        }
        printf("\n");
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
            sendbuf[i][j] = i;
        }
    }

    sleep(1);

    for (int i = 0; i < procs; i++)
    {
        printf("Rank %d send -> ", rank);

        for (int j = 0; j < number; j++)
        {
            printf("%d ", sendbuf[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    sleep(1);

    double start, end;

    //printf("Rank -> %d Buffer -> %c\n", rank, sendbuf[0]);
    start = MPI_Wtime();
    My_AlltoAll(sendbuf, recvbuf1, number, rank, procs, status, request);
    end = MPI_Wtime();

    sleep(1);

    if (rank == 0)
    {
        printf("My AlltoAll working -> %lf\n", (end - start));
        sleep(1);
    }


    start = MPI_Wtime();
    MPI_Alltoall(sendbuf, procs, MPI_INT, recvbuf2, procs, MPI_INT, MPI_COMM_WORLD);
    end = MPI_Wtime();


    if (rank == 0)
    {
        printf("MPI_Alltoall working -> %lf\n", (end - start));
    }

    sleep(1);

    MPI_Finalize();

}