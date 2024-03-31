#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define SIZE_MAX 1024

int main(int argc, char* argv[]) {
    int opt, i, strsize = 0;
    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
        case 's':
            strsize = atoi(optarg);
            break;
        default:
            fprintf(stderr, "»спользование: %s [-n process_id]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    int rank, tag = 99;
    char recvbuf[strsize];
    char sendbuf[strsize];
    for (i = 0; i < strsize - 1; i++) {
        sendbuf[i] = 'a';
    }
    MPI_Init(&argc, &argv);
    MPI_Status status;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double start_time = MPI_Wtime();
    if (rank == 0) {
        MPI_Send(sendbuf, strsize, MPI_CHAR, 1, tag, MPI_COMM_WORLD);
        fprintf(stdout, "buf=%s size=%d\n", sendbuf, strsize);
    }
    else if (rank == 1) {
        MPI_Recv(recvbuf, strsize, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
        fprintf(stdout, "recvbuf=%s\n", recvbuf);
    }
    double end_time = MPI_Wtime();
    fprintf(stdout, "rank=%d time=%f\n", rank, end_time - start_time);
    MPI_Finalize();
}
