/*
 *   1. С помощью неблокируемых операций точечного взаимодействия
 *      реализовать каждую из функций коллективного взаимодействия
 *      MPI_Scatter - передает сообщение от процесса с рангом "root"
 *      всем другим процессам коммуникатора
 *      # slide 30
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define MAX_BUF_SIZE 8388608   // 8 Mb

int main(int argc, char* argv[])
{
    int root_rank, rank, n;
    int opt, pr = 0;
    int i, j, message_slice = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char** messages = (char**)malloc(n * sizeof(char*));
    char* recvbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
    char* sendbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));


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

                if (rank == root_rank) {
                    for (i = 0; i < n; i++) {
                        messages[i] = (char*)malloc(message_slice * sizeof(char));
                        // Filling an arrays with data
                        for (j = 0; j < message_slice; j++) {
                            // ASCII
                            messages[i][j] = (char)(65 + i);
                            sendbuf[i*message_slice+j]=(char)(65 + i);
                        }
                    }
                }
                break;
            default:
                fprintf(stderr, "Invalid parameters. Set option -r <root_rank> -s <size>\n");
                exit(EXIT_FAILURE);
        }
    }
    if (argc != 5) {
        fprintf(stderr, "Invalid parameters count. Set option -r <root_rank> -s <size>\n");
        exit(EXIT_FAILURE);
    }
    // Output only in the mode of particular message transmission
    if (pr == 0 & rank == root_rank) {
        int i;
        printf("# Root_rank [%d], message <", rank);
        for (i = 0; i < n; i++ ){
            printf("%.*s", message_slice, messages[i]);
        }
        printf(">\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);

    double start_time, end_time, elapsed_time;

    start_time= MPI_Wtime();
    if (rank == root_rank) {
        MPI_Request requests_send[n];
        int i;
        for (i = 0; i < n; i++) {
            MPI_Isend(messages[i], message_slice, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests_send[i]);
        }
    }
    MPI_Request request_recv;
    MPI_Irecv(recvbuf, message_slice, MPI_CHAR, root_rank, 0, MPI_COMM_WORLD, &request_recv);
    MPI_Wait(&request_recv, MPI_STATUSES_IGNORE);
    end_time = MPI_Wtime();
    elapsed_time = end_time - start_time;
    // printf("Time elapsed with non-blocking operations: %f\n", end_time-start_time);

    if (pr == 0) {
        printf("[%d] %s\n", rank, recvbuf);
    }

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
        printf("Average time elapsed PI_Scatter with non-blocking operations: %lf\n", temp);
    }

    // ******* Library function MPI_Gather ******* //
    start_time = MPI_Wtime();
    MPI_Scatter(sendbuf, message_slice, MPI_CHAR, recvbuf, message_slice, MPI_CHAR, root_rank, MPI_COMM_WORLD);
    //printf("[%d] %s\n", rank, recvbuf);
    end_time = MPI_Wtime();
    elapsed_time = end_time-start_time;
    /* if (rank == 0){
        printf("# Result of the operation MPI_Scatter\n");
        }
    printf("[%d] %s\n", rank, recv); */
    //printf("Time elapsed wit operations: %f\n", end_time-start_time);

    // ******* Calculating the average value of the function ******** //
    MPI_Gather(&elapsed_time, 1, MPI_DOUBLE, res, 1, MPI_DOUBLE, root_rank, MPI_COMM_WORLD);
    if (rank == root_rank)
    {
        int i;
        double temp = res[0];
        for (i = 1; i < n; i++)
        {
            temp = (temp + res[i])/2;
        }
        printf("Average time elapsed with MPI_Scatter: %lf\n", temp);
    }

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}