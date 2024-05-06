#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "iostream"
#include <time.h>
using namespace std;

#define SEND_ROOT 0

void custom_sync(int size, int rank)
{
    sleep(size / (rank + 1));
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0)
        printf("===================================================\n");
}
int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;

    if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
        perror("Error at MPI_Comm_rank!\n");
    if (MPI_Comm_size(MPI_COMM_WORLD, &size))
        perror("Error at MPI_Comm_size!\n");

    //=================================[ SENDBUF INIT ]================================
    int* sendbuf = new int[size];
    for (int i = 0; i < size; i++)
        sendbuf[i] = size * rank + i;
    
    int* sendbuf_for_scatter = new int[size+1];
    for (int i = 0; i < size+1; i++)
        sendbuf_for_scatter[i] = size * rank + i;

    sleep(size / (rank + 1));
    printf("[SEND] rank=%d, arr: [ ", rank);
    for (int i = 0; i < size; i++)
        printf("%d ", sendbuf[i]);
    printf("]\n");
    MPI_Barrier(MPI_COMM_WORLD);

    sleep(size / (rank + 1));
    printf("[SEND FOR REDUCE_SCATTER] rank=%d, arr: [ ", rank);
    for (int i = 0; i < size+1; i++)
        printf("%d ", sendbuf_for_scatter[i]);
    printf("]\n");
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == 0)
        printf("===================================================\n");
    //=================================[ SENDBUF INIT ]==============================
    


    //=================================[ REDUCE ]====================================
    int* recvbuf1 = new int[size];

    MPI_Reduce(sendbuf, recvbuf1, size, MPI_INT, MPI_SUM, SEND_ROOT, MPI_COMM_WORLD);
    if (rank == SEND_ROOT)
    {
    printf("[REDUCE] [RECV] rank=%d, arr: [ ", rank);
    for (int i = 0; i < size; i++)
        printf("%d ", recvbuf1[i]);
    printf("]\n");
    }

    //=================================[ REDUCE ]====================================
    custom_sync(size, rank);



    //==============================[ REDUCE_SCATTER ]===============================
    int *count = new int[size];
    for (int i = 0; i < size; i++)
        count[i] = (i == 0) ? 2: 1;

    int* recvbuf2 = new int[size];
    MPI_Reduce_scatter(sendbuf_for_scatter, recvbuf2, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    printf("[REDUCE_SCATTER] [RECV] rank=%d, arr: [ ", rank);
    for (int i = 0; i < count[rank]; i++)
        printf("%d ", recvbuf2[i]);
    printf("]\n");
    //==============================[ REDUCE_SCATTER ]===============================
    custom_sync(size, rank);



    //==============================[ ALL_REDUCE ]===================================
    int* recvbuf3 = new int[size];
    MPI_Allreduce(sendbuf, recvbuf3, size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("[ALL_REDUCE] [RECV] rank=%d, arr: [ ", rank);
    for (int i = 0; i < size; i++)
        printf("%d ", recvbuf3[i]);
    printf("]\n");
    //==============================[ ALL_REDUCE ]===================================
    custom_sync(size, rank);



    //=================================[ SCAN ]======================================
    int* recvbuf4 = new int[size];
    MPI_Scan(sendbuf, recvbuf4, size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("[SCAN] [RECV] rank=%d, arr: [ ", rank);
    for (int i = 0; i < size; i++)
        printf("%d ", recvbuf4[i]);
    printf("]\n");
    //=================================[ SCAN ]======================================


    MPI_Finalize();
    return 0;
}
