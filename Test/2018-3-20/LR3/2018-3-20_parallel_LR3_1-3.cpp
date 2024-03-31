#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
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

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int number, err;
    err = getSizeData(argc, argv, &number);


    int rank, procs;
    char proc_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    MPI_Request request;
    MPI_Status status;

    int* recvbuf2 = new int[number];

    int* sendbuf = new int[number];

    for (int i = 0; i < number; i++)
    {
        sendbuf[i] = rank;
    }

    sleep(1);

    printf("Rank %d send -> ", rank);

    for (int j = 0; j < number; j++)
    {
        printf("%d ", sendbuf[j]);
    }
    printf("\n\n");

    sleep(1);

    double start, end;

    //start = MPI_Wtime();

    int* count = new int[procs];

    for (int i = 0; i < procs; i++)
    {
        count[i] = 1;
    }


    if (number > procs)
    {
        int rez = number - procs;
        count[1] = 1 + rez;
    }

    MPI_Reduce_scatter(sendbuf, recvbuf2, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    printf("Rank %d recv -> ", rank);

    for (int i = 0; i < count[rank]; i++)
    {
        printf("%d ", recvbuf2[i]);
    }
    printf("\n");


    //end = WPI_Wtime();

    MPI_Finalize();

}