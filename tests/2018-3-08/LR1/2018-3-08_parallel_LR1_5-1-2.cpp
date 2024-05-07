/*
 *   5. Реализовать при помощи отправки сообщений типа "точка-точка" следующие схемы коммуникации процессов:
 *      1) Передача данных по кольцу - “сдвиг”(одновременная посылка и прием сообщений)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define MAX_BUF_SIZE 65536      // 64 Mb

int main(int argc, char *argv[])
{
    int rank, n, rank_from, rank_to, opt, first_rank;
    char* sendbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
    char* recvbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Calculation of ranks for circular exchange
    rank_from = (n + rank - 1) % n;
    rank_to = (rank + 1) % n;

    sprintf(sendbuf, "Hello from %d", rank);

    MPI_Sendrecv(sendbuf, strlen(sendbuf), MPI_CHAR, rank_to,   1,
                 recvbuf, MAX_BUF_SIZE      , MPI_CHAR, rank_from, MPI_ANY_TAG,
                 MPI_COMM_WORLD, &status);

    // Result format:
    // Rank [] received a message from the left () and sent message to the right ()
    printf("(%d) --> [%d] --> (%d) message: \"%s\"\n", rank_from, rank, rank_to, recvbuf);

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}