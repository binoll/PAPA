#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int i, opt, size, rank, len = 3;
    MPI_Op op;
    int* sendbuf = NULL;
    int* recvbuf = NULL;

    while (((opt = getopt(argc, argv, "spml")) != -1)) {
        switch(opt) {
            case 's':
                op = MPI_SUM;
                break;
            case 'p':
                op = MPI_PROD;
                break;
            case 'm':
                op = MPI_MAX;
                break;
            case 'l':
                op = MPI_LAND;
                break;
            default:
                fprintf(stderr, "Invalid arguments\n");
                return 1;
        }
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    sendbuf = (int*)malloc(len * sizeof(int*));
    recvbuf = (int*)malloc(len * sizeof(int*));

    for (i = 0; i < len; ++i) {
        sendbuf[i] = rank * len + i;
        printf("%d rank [%d]: %d\n", rank, i + 1, sendbuf[i]);
    }

    MPI_Allreduce(sendbuf, recvbuf, len, MPI_INT, op, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    for (i = 0; i < len; ++i) {
        printf("%d rank result [%d]: %d\n", rank, i + 1, recvbuf[i]);
    }


    free(sendbuf);
    free(recvbuf);
    MPI_Finalize();
}
