/*
 *   1. С помощью неблокируемых операций точечного взаимодействия
 *      реализовать каждую из функций коллективного взаимодействия
 *      MPI_Bcast - передает сообщение от процесса с рангом "root"
 *      всем другим процессам коммуникатора
 *      # slide 28
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define MAX_BUF_SIZE 8388608   // 1 Mb

int main(int argc, char* argv[])
{

    int root_rank, rank, n;
    int opt, pr = 0;

    char* sendbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
    char* recvbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));

    char* text = "Hello, world!";
    int message_slice = strlen(text);
    char* message = (char*)malloc(strlen(text)+1);
    strcpy(message, text);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while ((opt = getopt(argc, argv, "r:s:")) != -1) {
        switch(opt) {
            case 'r':
                // The parameter specifies the master rank in [master-slave]
                root_rank = atoi(optarg);
                if (root_rank < 0 || root_rank >= n) {
                    fprintf(stderr, "Invalid parameters. Root rank must belong to the segment [0,%d]\n", n);
                    exit(EXIT_FAILURE);
                    }
                break;
            case 's':
                // The parameter specifies the size of the message to be sent
                // The content of the message is not specified - only the size
                // [ Optional for time calculations ]
                message_slice = atoi(optarg);
                if (message_slice > 10) {
                    pr = 1;
                }

                if (message_slice > MAX_BUF_SIZE) {
                    fprintf(stderr, "The size of the transmitted message exceeds the buffer size. "
                                    "Max buffer size: %d bytes\n", MAX_BUF_SIZE);

                    exit(EXIT_FAILURE);
                }
                message = (char*)malloc(message_slice * sizeof(char));

                // Filling an arrays with data
                for (int i = 0; i < message_slice; i++) {
                    // ASCII
                    message[i] = (char)(65 + root_rank);
                }
                break;
            default:
                fprintf(stderr, "Invalid parameters. Set option -r <root_rank> -s <size>\n");
                exit(EXIT_FAILURE);
        }
    }
    if (argc <= 1 || argc >5) {
        fprintf(stderr, "Invalid parameters count. Set option -r <root_rank> -s <size>\n");
        exit(EXIT_FAILURE);
    }
    strcpy(sendbuf, message);
    MPI_Request request_recv;
    double start_time, end_time;

    if (rank == root_rank) {
        start_time = MPI_Wtime();
        int i;
        MPI_Status status_send[n];
        MPI_Request requests_send[n];
        // Output only in the mode of particular message transmission
        if (pr == 0) {
            printf("# Root_rank [%d], message <%.*s>\n", rank, message_slice, sendbuf);
        }

        // Sending messages to all other processes
        for (i = 0; i < n; i++) {
            MPI_Isend(sendbuf, message_slice, MPI_CHAR, (root_rank + i) % n, 0, MPI_COMM_WORLD, &requests_send[i]);
        }
        MPI_Irecv(recvbuf, message_slice, MPI_CHAR, root_rank, 0, MPI_COMM_WORLD, &request_recv);
        MPI_Wait(&request_recv, MPI_STATUSES_IGNORE);
        end_time   = MPI_Wtime();
        printf("# Time elapsed with non-blocking operations: %f\n", end_time-start_time);
    }
    else {
        MPI_Irecv(recvbuf, message_slice, MPI_CHAR, root_rank, 0, MPI_COMM_WORLD, &request_recv);
        MPI_Wait(&request_recv, MPI_STATUSES_IGNORE);
    }

    if (pr == 0) {
        printf("[%d] %s\n", rank, recvbuf);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // ******* Library function MPI_Bcast ******* //
    start_time = MPI_Wtime();
    MPI_Bcast(sendbuf, message_slice, MPI_CHAR, root_rank, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    // printf("Time elapsed with Bcast: %f\n", elapsed_time);

    // ******* Calculating the average value of the function ******** //
    double* res = (double*)malloc(n * sizeof(double));
    MPI_Gather(&elapsed_time, 1, MPI_DOUBLE, res, 1, MPI_DOUBLE, root_rank, MPI_COMM_WORLD);
    if (rank == root_rank)
    {
        int i;
        double temp = res[0];
        for (i = 1; i < n; i++)
        {
            temp = (temp + res[i])/2;
        }
        printf("# Average time elapsed with MPI_Bcast: %lf\n", temp);
    }

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}