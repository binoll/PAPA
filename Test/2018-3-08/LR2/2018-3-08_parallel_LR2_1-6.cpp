/*
 *   1. С помощью неблокируемых операций точечного взаимодействия
 *      реализовать каждую из функций коллективного взаимодействия
 *      MPI_Alltoall - передает сообщение от процесса с рангом "root"
 *      всем другим процессам коммуникатора
 *      # slide 32
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

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char* message;
    char** recvbuf = (char**)malloc(n * sizeof(char*));
    char* sendbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
    char** messages = (char**)malloc(n * sizeof(char*));

    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch(opt) {
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

                for (i = 0; i < n; i++) {
                    messages[i] = (char*)malloc(message_slice * sizeof(char));
                    // Filling an arrays with data
                    for (j = 0; j < message_slice; j++) {
                        // ASCII
                        messages[i][j] = (char)(65 + i * message_slice + rank * n * message_slice);
                        sendbuf[i*message_slice+j]=(char)(65 + i * message_slice + rank *n * message_slice);
                    }
                }
                break;
            default:
                fprintf(stderr, "Invalid parameters. Set option -s <size>\n");
                exit(EXIT_FAILURE);
        }
    }

    if (argc != 3) {
        fprintf(stderr, "Invalid parameters count. Set option -s <size>\n");
        exit(EXIT_FAILURE);
    }
    if (rank == 0 & pr == 0) {
        printf("# Source data on each process\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);
    // Output only in the mode of particular message transmission
    if (pr == 0) {

       printf("[%d] %.*s\n", rank, message_slice * n, sendbuf);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // Output only in the mode of particular message transmission
    /* if (pr == 0) {
        for (i = 0; i<n; i++) {
            printf("[%d] %.*s\n", rank, message_slice, messages[i]);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD); */

    // Memory allocation of the receiving buffer
    for (i = 0; i<n; i++){
        recvbuf[i] = (char*)malloc(message_slice * sizeof(char));
    }

    double start_time, end_time, elapsed_time;
    MPI_Request requests_send[n], requests_recv[n];
    MPI_Status status_recv[n];

    start_time= MPI_Wtime();
    for (i = 0; i < n; i++)
    {
        MPI_Isend(messages[i], message_slice, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests_send[i]);
        MPI_Irecv(recvbuf[i], message_slice, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests_recv[i]);
    }
    MPI_Waitall(n, requests_recv, status_recv);
    end_time = MPI_Wtime();
    elapsed_time = end_time - start_time;
    // printf("Time elapsed with non-blocking operations: %f\n", end_time-start_time);

    if (rank == 0 & pr == 0 ){
        printf("# Result of the operation MPI_Alltoall with non-blocking operations\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);
    // Output only in the mode of particular message transmission
    if (pr == 0) {
        printf("[%d] ", rank);
        for (i=0;i<n;i++){
            printf("%.*s", message_slice, recvbuf[i]);
        }
        printf("\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // ******* Calculating the average value of the function ******** //
    double* res = (double*)malloc(n * sizeof(double));
    MPI_Gather(&elapsed_time, 1, MPI_DOUBLE, res, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if (rank == 0){
        int i;
        double temp = res[0];
        for (i = 1; i < n; i++){
            temp = (temp + res[i])/2;
        }
        printf("# Average time elapsed MPI_Alltoall with non-blocking operations: %lf\n\n", temp);
    }

    // ******* Library function MPI_Alltoall ******* //
    char* recv = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
    start_time = MPI_Wtime();
    MPI_Alltoall(sendbuf, message_slice, MPI_CHAR, recv, message_slice, MPI_CHAR, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    elapsed_time = end_time-start_time;
    if (rank == 0 & pr == 0){
        printf("# Result of the operation MPI_Alltoall\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if (pr == 0 ){
        printf("[%d] %s\n", rank, recv);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // ******* Calculating the average value of the MPI_Alltoall ******** //
    MPI_Gather(&elapsed_time, 1, MPI_DOUBLE, res, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if (rank == 0){
        int i;
        double temp = res[0];
        for (i = 1; i < n; i++)
        {
            temp = (temp + res[i])/2;
        }
        printf("# Average time elapsed with MPI_Alltoall: %lf\n", temp);
    }

    MPI_Finalize();
    exit(EXIT_SUCCESS);
}