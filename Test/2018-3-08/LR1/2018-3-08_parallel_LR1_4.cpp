/*
 *   4. Сравнить время реализации пересылок данных между двумя
 *      выделенными процессорами с блокировкой и без блокировки.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define MAX_BUF_SIZE 131072     // 128 Mb

int main(int argc, char *argv[])
{
    int rank, opt, count;
    char** messages = (char**)malloc(2 * sizeof(char*));
    char* sendbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
    char* recvbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
    int tag0 = 0, tag1 = 1;
    double time_start, time_stop;
    // Parameter pr allows to hide the output of the message content when calculating the time
    int i, n, pr = 0;
    int message_slice = MAX_BUF_SIZE;

    MPI_Status status[2];
    MPI_Request requests[2];

    while ((opt = getopt(argc, argv, "s:r:g:")) != -1) {
        switch(opt) {
            case 's':
                // The parameter specifies the message to be sent [process 0]
                messages[0] = (char*)malloc(strlen(optarg));
                strcpy(messages[0], optarg);
                break;
            case 'r':
                // The parameter specifies the message to be sent [process 1]
                messages[1] = (char*)malloc(strlen(optarg));
                strcpy(messages[1], optarg);
                break;
            case 'g':
                // The parameter specifies the size of the message to be sent
                // The content of the message is not specified - only the size
                // [ Optional for time calculations ]
                pr = 1;
                message_slice = atoi(optarg);

                if (message_slice > MAX_BUF_SIZE) {
                    fprintf(stderr, "The size of the transmitted message exceeds the buffer size. "
                                    "Max buffer size: %d bytes\n", MAX_BUF_SIZE);

                    exit(EXIT_FAILURE);
                }
                messages[0] = (char*)malloc(message_slice * sizeof(char));
                messages[1] = (char*)malloc(message_slice * sizeof(char));

                // Filling an arrays with data - 0 and 1
                for (i = 0; i < message_slice - 1; i++) {
                    messages[0][i] = '0';
                    messages[1][i] = '1';
                }
                break;
            default:
                fprintf(stderr, "Invalid parameters. Get-opt return value: 0%o\n", opt);
                exit(EXIT_FAILURE);
        }
    }
    if (argc <= 1) {
        fprintf(stderr, "Invalid parameters. Set options: -s <message1> -r <message2>\n");
        exit(EXIT_FAILURE);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);

    // Checking that exactly 2 nodes are interacting
    // Print error in each process
    if (n != 2) {
        fprintf(stderr, "Invalid parameter for the number of processes. Set: mpirun -np 2\n");
        exit(EXIT_FAILURE);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    strcpy(sendbuf, messages[rank]);

    // Output only in the mode of particular message transmission
    if (pr == 0) {
        printf("[%d] --> (%lu) %s\n", rank, strlen(sendbuf), sendbuf);
    }

    time_start = MPI_Wtime();
    if (rank == 0) {
        MPI_Isend(sendbuf, strlen(sendbuf), MPI_CHAR, 1, tag0, MPI_COMM_WORLD, &requests[0]);
        MPI_Irecv(recvbuf, MAX_BUF_SIZE, MPI_CHAR, 1, tag1, MPI_COMM_WORLD, &requests[1]);
    }
    else if (rank == 1) {
        MPI_Irecv(recvbuf, MAX_BUF_SIZE, MPI_CHAR, 0, tag0, MPI_COMM_WORLD, &requests[0]);
        MPI_Isend(sendbuf, strlen(sendbuf), MPI_CHAR, 0, tag1, MPI_COMM_WORLD, &requests[1]);
    }
    MPI_Waitall(2, requests, status);

    // Output only in the mode of particular message transmission
    if (pr == 0) {
        printf("[%d] <-- (%lu) %s\n", rank, strlen(recvbuf), recvbuf);
    }

    time_stop = MPI_Wtime();
    printf("Elapsed time in [%d]: %f sec\n", rank, (time_stop - time_start)/2);

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}
