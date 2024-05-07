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

    void* recvbuf2[100];

    int* sendbuf = new int[procs];
    char* sendbuf2 = new char[procs];

    double start, end;

    //start = MPI_Wtime();
    int position = 0;

    if (rank == 0)
    {

        for (int i = 0; i < procs; i++)
        {
            sendbuf[i] = rank + 1;
            sendbuf2[i] = 'a';
        }

        MPI_Pack(sendbuf, procs, MPI_INT, recvbuf2, 100, &position, MPI_COMM_WORLD);
        MPI_Pack(sendbuf2, procs, MPI_CHAR, recvbuf2, 100, &position, MPI_COMM_WORLD);

       //MPI_Bcast(recvbuf2, 100, MPI_PACKED, 0, MPI_COMM_WORLD);

        printf("Rank %d send -> ", rank);

        for (int j = 0; j < procs; j++)
        {
            printf("%d ", sendbuf[j]);
        }
        printf("\n");

        printf("Rank %d send -> ", rank);

        for (int j = 0; j < procs; j++)
        {
            printf("%c ", sendbuf2[j]);
        }
        printf("\n");
    }

    MPI_Bcast(recvbuf2, 100, MPI_PACKED, 0, MPI_COMM_WORLD);
    MPI_Unpack(recvbuf2, 100, &position, sendbuf, procs, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(recvbuf2, 100, &position, sendbuf2, procs, MPI_CHAR, MPI_COMM_WORLD);

    if (rank != 0)
    {

        printf("Rank %d recv -> ", rank);

        for (int j = 0; j < procs; j++)
        {
            printf("INT -> %d ", sendbuf[j]);
        }
        printf("\n");
        printf("Rank %d recv -> ", rank);
        for (int j = 0; j < procs; j++)
        {
            printf("CHAR -> %c ", sendbuf2[j]);
        }
        printf("\n");
    }


    MPI_Finalize();

}