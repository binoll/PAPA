/*
 *   5. Реализовать при помощи отправки сообщений типа "точка-точка" следующие схемы коммуникации процессов:
 *      2. master-slave (все процессы общаются с одним).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define MAX_BUF_SIZE 65536      // 64 Mb

int main(int argc, char *argv[])
{
    int rank, i, n, opt, master;
    char* sendbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
    char* recvbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
    MPI_Status status;

    while ((opt = getopt(argc, argv, "m:")) != -1) {
        switch (opt) {
            case 'm':
                // The parameter specifies the master rank in [master-slave]
                master = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Invalid parameters. Getopt return value: 0%o\n", opt);
                exit(EXIT_FAILURE);
        }
    }
    if (argc <= 1) {
        fprintf(stderr, "Invalid parameters. Set option -m <master>\n");
        exit(EXIT_FAILURE);
    }

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == master) {
        sprintf(sendbuf, "Hello from master %d", rank);
        for (i = 1; i < n; i++) {
            MPI_Sendrecv(sendbuf, strlen(sendbuf), MPI_CHAR, (master + i) % n,   1,
                         recvbuf, MAX_BUF_SIZE,       MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG,
                         MPI_COMM_WORLD, &status);
            printf("[%d] <-- %s\n", rank, recvbuf);
        }
    }
    else {
        sprintf(sendbuf, "Hello from slave %d", rank);
        MPI_Sendrecv(sendbuf, strlen(sendbuf), MPI_CHAR, master, 1,
                     recvbuf, MAX_BUF_SIZE,       MPI_CHAR, master, 1,
                     MPI_COMM_WORLD, &status);
        printf("[%d] <-- %s\n", rank, recvbuf);
    }

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}