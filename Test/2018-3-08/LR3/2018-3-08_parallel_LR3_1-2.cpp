/*
 *   1. Разработать параллельную программу, демонстрирующую применение глобальных операций в MPI
 *      MPI_Allreduce
 *      # slide 35
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define MAX_BUF_SIZE 8388608   // 8 Mb

int main(int argc, char* argv[])
{
    int rank,root_rank, n;
    int opt, pr = 0;
    int i, j, message_slice = 0;
    char operation_message[7];
    int* sendbuf, *recvbuf;

    MPI_Op operation;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while ((opt = getopt(argc, argv, "s:o:")) != -1) {
        switch(opt) {
            case 'o':
                strcpy(operation_message, optarg);
                if  (!strcmp(optarg, "MPI_MAX"))
                { operation = MPI_MAX; }
                else if (!strcmp(optarg, "MPI_MIN"))
                { operation = MPI_MIN; }
                else if (!strcmp(optarg, "MPI_SUM"))
                { operation = MPI_SUM; }
                else if (!strcmp(optarg, "MPI_PROD"))
                { operation = MPI_PROD; }
                else {
                    fprintf(stderr, "Invalid parameters. Unknown operation in option -o <operation>\n."
                                    " Use MPI_MIN or MPI_MAX or MPI_SUM or MPI_PROD");
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                // The parameter specifies the size of the message to be sent
                // The content of the message is not specified - only the size
                message_slice = atoi(optarg);
                if (message_slice > 10) {
                    pr = 1;
                }

                if (message_slice > MAX_BUF_SIZE) {
                    fprintf(stderr, "The size of the transmitted message exceeds the buffer size. "
                                    "Max buffer size: %d bytes\n", MAX_BUF_SIZE);

                    exit(EXIT_FAILURE);
                }
                sendbuf = (int*)malloc(message_slice * sizeof(int));
                recvbuf = (int*)malloc(message_slice * sizeof(int));

                // Filling an arrays with data
                for (int i = 0; i < message_slice; i++) {
                    sendbuf[i] = message_slice * rank + i;
                }
                break;
            default:
                fprintf(stderr, "Invalid parameters count. Set option -s <size> -o <operation>\n");
                exit(EXIT_FAILURE);
        }
    }
    if (argc !=5) {
        fprintf(stderr, "Invalid parameters count. Set option -s <size> -o <operation>\n");
        exit(EXIT_FAILURE);
    }

    if (rank == 0 & pr == 0) {
        printf("# Source data on each process\n");
    }
    // To correctly display the result on the screen by printf()
    MPI_Barrier(MPI_COMM_WORLD);
    // Output only in the mode of particular message transmission
    if (pr == 0) {
        printf("[%d] < ", rank);
        for (i = 0; i < message_slice; i++)
        {
            printf("%2d ", sendbuf[i]);
        }
        printf(">\n");
    }
    // To correctly display the result on the screen by printf()
    MPI_Barrier(MPI_COMM_WORLD);

    // ******* Library function MPI_Allreduce with user operation ******* //
    MPI_Allreduce(sendbuf, recvbuf, message_slice, MPI_INT, operation, MPI_COMM_WORLD);
    if (rank == root_rank) {
        printf("# Result of the operation MPI_Allreduce with reduce operation - <%s> \n", operation_message);
    }
    // To correctly display the result on the screen by printf()
    MPI_Barrier(MPI_COMM_WORLD);
    // Output only in the mode of particular message transmission
    if (pr == 0) {
        printf("[%d] < ", rank);
        for (i = 0; i < message_slice; i++)
        {
            printf("%2d ", recvbuf[i]);
        }
        printf(">\n");
    }

    MPI_Finalize();
    exit(EXIT_SUCCESS);
}