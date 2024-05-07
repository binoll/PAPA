#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

using namespace std;

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, procs;
    char proc_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    MPI_Request request;
    MPI_Status status;

    int* recvbuf2 = new int[procs];

    int* sendbuf = new int[procs];

    for (int i = 0; i < procs; i++)
    {
        sendbuf[i] = rank;
    }

    sleep(1);

    printf("Rank %d send -> ", rank);

    for (int j = 0; j < procs; j++)
    {
        printf("%d ", sendbuf[j]);
    }
    printf("\n\n");

    sleep(1);

    double start, end;

    //start = MPI_Wtime();

    MPI_Scan(sendbuf, recvbuf2,procs, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    printf("Rank %d recv -> ", rank);

    for (int j = 0; j < procs; j++)
    {
        printf("%d ", recvbuf2[j]);
    }
    printf("\n");


    //end = WPI_Wtime();

    MPI_Finalize();

}