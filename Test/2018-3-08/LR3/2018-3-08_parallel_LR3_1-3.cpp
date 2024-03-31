/*
 *   1. Разработать параллельную программу, демонстрирующую применение глобальных операций в MPI
 *      MPI_Reduce_scatter
 *      # slide 36
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define MAX_BUF_SIZE 8388608   // 8 Mb

int main(int argc, char* argv[])
{
    int rank, n;
    int opt, pr = 0;
    int i, j, message_slice = 0;
    char operation_message[7];
    int* sendbuf, *recvbuf;

    MPI_Op operation;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int count[n];
    char count_input[2*n];

    while ((opt = getopt(argc, argv, "s:o:c:")) != -1) {
        switch(opt) {

            case 'o':
                // Determination of the required operation
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

                // Filling an arrays with data
                for (int i = 0; i < message_slice; i++) {
                    sendbuf[i] = message_slice * rank + i;
                }
                break;
            case 'c':
                // Determining the amount of information transmitted to each process
                // Count = {1, 2, 3} - which 1 - amount of information in 0 process, 2 - amount of information in 1 process etc.
                // Input -c "1 2 3"
                strcpy(count_input, optarg);
                int total_operations = 0;

                for (i = 0; i < strlen(optarg); i++) {
                    count[i]=atoi(&count_input[2*i]);
                    total_operations += count[i];
                }

                int total_elements_in_count = (strlen(optarg) - 1)/2 + 1;

                if (n != total_elements_in_count) {
                    fprintf(stderr, "Invalid parameters <count> -c '1 0 1'. <Count> should determine the distribution of data across all processes\n");
                    exit(EXIT_FAILURE);
                }

                if (total_operations > message_slice) {
                    fprintf(stderr, "Invalid parameters <count>. The transmitted data should be enough for all processes\n");
                    exit(EXIT_FAILURE);
                }

                break;
            default:
                fprintf(stderr, "Invalid parameters count. Set option -s <size> -o <operation> -c <count> '1 1 1'\n");
                exit(EXIT_FAILURE);
        }
    }
    if (argc !=7) {
        fprintf(stderr, "Invalid parameters count. Set option -s <size> -o <operation> -c <count> '1 1 1'\n");
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
    MPI_Barrier(MPI_COMM_WORLD);
    // ******* Library function MPI_Reduce_scatter with user operation ******* //
    recvbuf = (int*)malloc(count[rank] * sizeof(int));
    // To correctly display the result on the screen by printf()
    //MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce_scatter(sendbuf, recvbuf, count, MPI_INT, operation, MPI_COMM_WORLD);
    if (!rank) {
        printf("# Result of the operation MPI_Reduce_scatter with reduce operation - <%s> \n", operation_message);
    }
    // To correctly display the result on the screen by printf()
    MPI_Barrier(MPI_COMM_WORLD);
    // Output only in the mode of particular message transmission
    if (pr == 0 && count[rank] > 0) {
        printf("[%d] < ", rank);
        for (i = 0; i < count[rank]; i++)
        {
            printf("%2d ", recvbuf[i]);
        }
        printf(">\n");
    }

    MPI_Finalize();
    exit(EXIT_SUCCESS);
}