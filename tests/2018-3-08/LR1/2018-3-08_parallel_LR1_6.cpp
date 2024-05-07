/*
 *   6. С использованием  неблокирующих операций осуществить транспонирование
 *      квадратной матрицы, распределенной между процессорами по строкам.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, n;
    int i, j, k;
    char sendbuf[n];
    char recvbuf[n];

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int total_exchange = n*n;
    MPI_Status status[total_exchange];
    MPI_Request requests[total_exchange];

    for (j = 0; j < total_exchange; j++) {
        requests[j] = MPI_REQUEST_NULL;
    }

    // Filling the matrix row with char values
    for(k = 0; k < n; k++) {
        // ASCII 65 - letter A
        sendbuf[k] = (char)(65 + k + n * rank);
    }
    // Result format:
    // [rank] string in process
    printf("[%d] %s\n", rank, sendbuf);

    // The function Barrier is used to correctly output the original and the resulting matrix
    MPI_Barrier(MPI_COMM_WORLD);
    // Each of the processes sends letter to other process according to the iteration
    // Including myself
    for (i = 0; i < n; i++) {
        MPI_Isend(&sendbuf[i], 1, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests[rank * n + i]);
        MPI_Irecv(&recvbuf[i], 1, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests[rank * n + i]);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) { printf("\n"); }

    MPI_Waitall(total_exchange, requests, status);

    // asterisk (*) to pass the width specifier/precision of recvbuf
    // Result format:
    // [rank] string in process
    printf("[%d] %.*s\n", rank, n, recvbuf);

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}