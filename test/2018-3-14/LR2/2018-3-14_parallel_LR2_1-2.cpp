
/*
    1. С помощью неблокируемых операций точечного взаимодействия реализовать каждую
        из функций коллективного взаимодействия (MPI_Barrier, MPI_Bcast, MPI_Gather,
        MPI_Scatter, MPI_Allgather, MPI_Alltoall).
    2. Произвести экспериментальное сравнение времени выполнения разработанных функций
        с функциями коллективного взаимодействия для разного количества передаваемых данных.

    MPI_Bcast
*/

#include "lr2-1.h"

#define ROOT_RANK root_rank

int main(int argc, char* argv[])
{
    int rank, n, root_rank, size, i;
    double start, stop;
    char *sendbuf, *recvbuf;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    getopt_rs(argc, argv, n, &root_rank, &size);

    if (rank == ROOT_RANK) {
        sendbuf = (char*)malloc(size * sizeof(char));

        for(i = 0; i < size; i++) {
           sendbuf[i] = (char)(65 + i % 63);
        }

        printf("[%d] (%d byte) %.*s\n\n", rank, size, size <= MAX_RPINT ? size : 0, sendbuf);
    }

    recvbuf = (char*)malloc(size * sizeof(char));

    MPI_Request request;

    start = MPI_Wtime();
    // ********************************** MPI_Bcast **********************************

    if (rank == ROOT_RANK) {
        MPI_Status status[n];
        MPI_Request requests[n];

        for (i = 0; i < n; i++) {
            MPI_Isend(sendbuf, size, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests[i]);
        }

        // MPI_Waitall(n, requests, status); <- А.В. сказал, что можно убрать
    }

    MPI_Irecv(recvbuf, size, MPI_CHAR, ROOT_RANK, 0, MPI_COMM_WORLD, &request);

    MPI_Wait(&request, MPI_STATUSES_IGNORE);

    // ********************************** MPI_Bcast **********************************
    stop = MPI_Wtime();

    printf("[%d] (%d byte) %.*s\n", rank, size, size <= MAX_RPINT ? size : 0, recvbuf);

    MPI_Wtime_avg_comparator(stop - start, n, rank, ROOT_RANK, sendbuf, size, MPI_CHAR, recvbuf, size, MPI_CHAR, MPI_COMM_WORLD, MPI_Bcast);

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}