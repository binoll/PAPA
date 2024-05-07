/*
 *   5. Реализовать при помощи отправки сообщений типа "точка-точка" следующие схемы коммуникации процессов:
 *      1) Передача данных по кольцу - “эстафетная палочка”
 *         (очередной процесс дожидается сообщения от предыдущего и потом посылает следующему)
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

    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
            case 'f':
                first_rank = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Invalid parameters. Get-opt return value: 0%o\n", opt);
                exit(EXIT_FAILURE);
        }
    }
    if (argc <= 1) {
        fprintf(stderr, "Invalid parameters. Set option -f <first_rank>\n");
        exit(EXIT_FAILURE);
    }

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Calculation of ranks for circular exchange
    rank_from = (n + rank - 1) % n;
    rank_to = (rank + 1) % n;

    sprintf(sendbuf, "Hello from %d", rank);

    if (rank == first_rank) {
        MPI_Send(sendbuf, strlen(sendbuf), MPI_CHAR, rank_to,   1,           MPI_COMM_WORLD);
        MPI_Recv(recvbuf, MAX_BUF_SIZE,       MPI_CHAR, rank_from, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }
    else {
        MPI_Recv(recvbuf, MAX_BUF_SIZE,    MPI_CHAR, rank_from, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Send(sendbuf, strlen(sendbuf), MPI_CHAR, rank_to,   1,           MPI_COMM_WORLD);
    }

    // Result format:
    // Rank [] received a message from the left () and sent message to the right ()
    printf("(%d) --> [%d] --> (%d) message: \"%s\"\n", rank_from, rank, rank_to, recvbuf);

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}