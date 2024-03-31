#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <time.h>
#define ROOT_RANK 0
using namespace std;


int** matmul(int** mat1, int**mat2, int N)
{
    int** res = new int*[N];
    for(int i = 0; i < N; i++)
        res[i] = new int[N];

    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
        {
            res[i][j] = 0;
            for(int k = 0; k < N; k++)
                res[i][j] += mat1[i][k] * mat2[k][j];
        }

    return res;
}

int** create_matrix(int N)
{
    int** result = new int*[N];
    for(int i = 0; i < N; i++)
        result[i] = new int[N];
    return result;
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;

    if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
        perror("Error at MPI_Comm_rank!\n");
    if (MPI_Comm_size(MPI_COMM_WORLD, &size))
        perror("Error at MPI_Comm_size!\n");

    int RECVSIZE = size*size, 
        SENDSIZE=size;

    int* sendbuf = new int[SENDSIZE];
    int* recvbuf = new int[RECVSIZE];

    int* back_sendbuf = new int[RECVSIZE];
    int* back_recvbuf = new int[SENDSIZE];
    

    for(int i = 0; i < SENDSIZE; i++)
        sendbuf[i] = size * rank + i;


    if(MPI_Gather(sendbuf, SENDSIZE, MPI_INT,
                  recvbuf, SENDSIZE, MPI_INT,
                  ROOT_RANK, MPI_COMM_WORLD))
        perror("Error at MPI_Gather!\n");

    // ================== PRINT TIME =====================//
    sleep(rank*size/10);

    printf("[SEND] <-- rank=%d, arr: [ ", rank);
    for (int i = 0; i < SENDSIZE; i++)
        printf("%d ", sendbuf[i]);
    printf("]\n");

    MPI_Barrier(MPI_COMM_WORLD);
    // ================== PRINT TIME =====================//
    
    
    if(rank == ROOT_RANK)
    {

        int** matrix = create_matrix(SENDSIZE);

        for(int k = 0; k < RECVSIZE;)
            for(int i = 0; i < SENDSIZE; i++)
                for(int j = 0; j < SENDSIZE; j++)
                {
                    matrix[i][j] = recvbuf[k];
                    k++;
                }

        delete[] recvbuf;

        int** result = matmul(matrix, matrix, SENDSIZE);

        for(int k = 0; k < RECVSIZE;)
            for (int i = 0; i < SENDSIZE; i++)
                for (int j = 0; j < SENDSIZE; j++)
                {
                    back_sendbuf[k] = result[i][j];
                    k++;
                }

        for(int i = 0;i < SENDSIZE; i++)
            delete result[i];
        delete[] result;
    }
    if(MPI_Scatter(back_sendbuf, SENDSIZE, MPI_INT,
                   back_recvbuf, SENDSIZE, MPI_INT,
                   ROOT_RANK, MPI_COMM_WORLD))
        perror("Error at MPI_Gather!\n");

    // ================== PRINT TIME =====================//
    sleep(rank*size / 10);

    printf("[RECV] --> rank=%d, arr: [ ", rank);
    for (int i = 0; i < SENDSIZE; i++)
        printf("%d ", back_recvbuf[i]);
    printf("]\n");

    MPI_Barrier(MPI_COMM_WORLD);
    // ================== PRINT TIME =====================//

    MPI_Finalize();
    return 0;
}
