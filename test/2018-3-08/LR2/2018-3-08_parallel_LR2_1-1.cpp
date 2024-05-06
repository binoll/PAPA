/*
 *   1. С помощью неблокируемых операций точечного взаимодействия
 *      реализовать каждую из функций коллективного взаимодействия
 *      MPI_Barrier - процессы блокируется до тех пор,
 *      пока все в коммуникаторе не достигнут этой процедуры.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
    int rank, n;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("[%d] Message before barrier\n", rank);

    if (rank == 0) {
        int i;
        MPI_Status status_recv[n - 1], status_send[n - 1];
        MPI_Request requests_recv[n - 1], requests_send[n - 1];

        // Receiving messages from all other processes
        for (i = 1; i < n; i++) {
            MPI_Irecv(NULL, 0, MPI_BYTE, i, 0, MPI_COMM_WORLD, &requests_recv[i-1]);
        }
        // Waits for all given MPI Requests to complete
        MPI_Waitall(n-1, requests_recv, status_recv);

        // Sending a message to all other processes
        for (i = 1; i < n; i++) {
            MPI_Isend(NULL, 0, MPI_BYTE, i, 1, MPI_COMM_WORLD, &requests_send[i-1]);
        }
        // Waits for all given MPI Requests to complete
        MPI_Waitall(n-1, requests_send, status_send);
    }
    else {
        MPI_Request request_recv, request_send;

        // Sending a message to main process
        MPI_Isend(NULL, 0, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &request_send);
        MPI_Wait(&request_send, MPI_STATUSES_IGNORE);

        // Receiving message from main process
        MPI_Irecv(NULL, 0, MPI_BYTE, 0, 1, MPI_COMM_WORLD, &request_recv);
        MPI_Wait(&request_recv, MPI_STATUSES_IGNORE);
    }
    printf("[%d] Continue work after barrier\n", rank);

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}