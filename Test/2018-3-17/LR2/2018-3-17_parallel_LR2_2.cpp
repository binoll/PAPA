#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <time.h>
#include <vector>
#define ROOT_RANK 0
using namespace std;


int** matmul(int** mat1, int** mat2, int N)
{
    int** res = new int* [N];
    for (int i = 0; i < N; i++)
        res[i] = new int[N];

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            res[i][j] = 0;
            for (int k = 0; k < N; k++)
                res[i][j] += mat1[i][k] * mat2[k][j];
        }

    return res;
}

int* copy(int* arr1, int* arr2, int size)
{
    for (int i = 0; i < size; i++)
        arr1[i] = arr2[i];
}

int** create_matrix(int N)
{
    int** result = new int* [N];
    for (int i = 0; i < N; i++)
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

    int RECVSIZE = size * size;


    int* sendbuf = new int[size];
    for (int i = 0; i < size; i++)
        sendbuf[i] = size * rank + i;
    int* recvbuf = new int[size];

    MPI_Alltoall(sendbuf, 1, MPI_INT, recvbuf, 1, MPI_INT, MPI_COMM_WORLD);
    
    int* tr_matr = new int[size * size];
    int* row = new int[size];

    MPI_Allgather(recvbuf, size, MPI_INT, tr_matr, size, MPI_INT, MPI_COMM_WORLD);
    
    int* result = new int[size];
    for (int i = 0; i < size; i++)
        result[i] = 0;
    for (int k = 0; k < size * size;)
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
            {
                result[i] += sendbuf[j] * tr_matr[k];
                k++;
            }
    
    // ================== PRINT TIME =====================//

    MPI_Barrier(MPI_COMM_WORLD);
    printf("[CALC] rank=%d, arr: [ ", rank);
    for (int i = 0; i < size; i++)
         printf("%d ", result[i]);
    printf("]\n");

    // ================== PRINT TIME =====================//


    MPI_Finalize();
    return 0;
}
