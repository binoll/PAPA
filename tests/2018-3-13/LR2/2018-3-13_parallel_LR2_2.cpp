#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <vector>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

using namespace std;

void matrixxmatrix()
{

    
    int size, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Status status;
    MPI_Request request;

    vector<int> recv_vector;

    int sendbuf1[size];
    int sendbuf2[size];

    int* recvbuf1 = new int(size);
    int* recvbuf2 = new int(size);


    for (int i = 0; i < size; i++)
    {
        sendbuf1[i] = (rank + 1) * (i + 5);
        sendbuf2[i] = (rank + 1) * (i + 3);
    }

    printf("first pocs rank %d send:", rank);
    for (int i = 0; i < size; i++)
    {
        printf("%d ", sendbuf1[i]);
    }
    printf("\n");
    sleep(2);

    printf("second pocs rank %d send:", rank);
    for (int i = 0; i < size; i++)
    {
        printf("%d ", sendbuf2[i]);
    }
    printf("\n");
    int* matrix = new int[size];
    for(int i = 0; i < size; i++)
    {
        int elem[] = {sendbuf2[i]};

        MPI_Allgather(elem, 1, MPI_INT, recvbuf1, 1, MPI_INT, MPI_COMM_WORLD);

        int elem1 = 0;
        for (int j = 0; j < size; j++)
        {
            elem1 += sendbuf1[j] * recvbuf1[j];
        }
        matrix[i] = elem1;
    }

    sleep(1);
    for (int i = 0; i < size; i++)
    {
        printf("%d ", matrix[i]);
    }
    printf("\n");

}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    char proc_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    double start, end;

    int time_sleep = 3;
    start = MPI_Wtime();

    matrixxmatrix();

    MPI_Finalize();

}
