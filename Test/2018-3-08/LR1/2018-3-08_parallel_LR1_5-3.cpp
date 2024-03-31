/*
 *   5. Реализовать при помощи отправки сообщений типа "точка-точка" следующие схемы коммуникации процессов:
 *      3. Пересылка данных от каждого процесса каждому.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define MAX_BUF_SIZE 65536      // 64 Mb

int main(int argc, char *argv[])
{
    int rank, n, i;
    char* sendbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
    char* recvbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    sprintf(sendbuf, "Hello from %d", rank);

    // Each of the processes sends to each of the n processes
    // Including myself
    for (i = 0; i < n; i++) {
        MPI_Sendrecv(sendbuf, strlen(sendbuf), MPI_CHAR, i, 1,
                     recvbuf, MAX_BUF_SIZE,       MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &status);
        printf("[%d] <-- %s\n", rank, recvbuf);
    }

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}