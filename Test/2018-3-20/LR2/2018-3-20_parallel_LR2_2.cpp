#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <vector>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

using namespace std;

void My_AllGather(int rank)
{

    int procs;
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    MPI_Status status;
    MPI_Request request;

    vector<int> recv_vector;

    int* recvbuf1 = new int(procs);
    int* recvbuf2 = new int(procs);
    int sendbuf1[procs];
    int sendbuf2[procs];

    for (int i = 0; i < procs; i++)
    {
        sendbuf1[i] = (rank + 1) * (i + 1);
        sendbuf2[i] = (rank + 1) * (i + 2);
    }

    printf("SEND1 rank %d -> ", rank);
    for (int i = 0; i < procs; i++)
    {
        printf("%d ", sendbuf1[i]);
    }
    printf("\n");
    sleep(2);

    printf("SEND2 rank %d -> ", rank);
    for (int i = 0; i < procs; i++)
    {
        printf("%d ", sendbuf2[i]);
    }
    printf("\n");
    int* matrix = new int[procs];
    for (int i = 0; i < procs; i++)
    {
        int elem[] = { sendbuf2[i] };

        MPI_Allgather(elem, 1, MPI_INT, recvbuf1, 1, MPI_INT, MPI_COMM_WORLD);

        int elem1 = 0;
        for (int j = 0; j < procs; j++)
        {
            elem1 += sendbuf1[j] * recvbuf1[j];
        }
        matrix[i] = elem1;
    }

    sleep(1);

    printf("rank %d -> ", rank);

    for (int i = 0; i < procs; i++)
    {
        printf("%d ", matrix[i]);
    }
    printf("\n");

    /*sleep(1);

    int* matrix = new int[procs];
    for (int i = 0; i < procs; i++)
    {
        int elem = 0;
        elem += sendbuf1[i] * recvbuf1[i];

        matrix[i] = elem;
    }

    printf("rank %d -> ", rank);

    for (int i = 0; i < procs; i++)
    {
        printf("%d ", matrix[i]);
    }
    printf("\n");*/
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, procs;
    char proc_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);


    double start, end;

    int time_sleep = 3;
    start = MPI_Wtime();


    //printf("Rank -> %d Buffer -> %c\n", rank, sendbuf[0]);
    My_AllGather(rank);


    /*if (rank == 0)
    {
        printf("Rank -> %d Buffer -> %c\n", rank, sendbuf[0]);
        My_Bcast(sendbuf, request);
    }
    else
    {
        char* recvbuf = new char(sizeof(sendbuf));

        MPI_Irecv(recvbuf, sizeof(recvbuf), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request[rank]);

        MPI_Wait(request, status);

        printf("Rank -> %d Buffer -> %c\n", rank, recvbuf[0]);
    }*/

    MPI_Finalize();

}