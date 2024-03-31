
/*
    1. С помощью неблокируемых операций точечного взаимодействия реализовать каждую
        из функций коллективного взаимодействия (MPI_Barrier, MPI_Bcast, MPI_Gather,
        MPI_Scatter, MPI_Allgather, MPI_Alltoall).
    2. Произвести экспериментальное сравнение времени выполнения разработанных функций
        с функциями коллективного взаимодействия для разного количества передаваемых данных.
    
    MPI_Allgather
*/

#include "lr2-1.h"

#define ROOT_RANK 0

int main(int argc, char* argv[])
{
    int rank, n, size ,i;
    double start, stop;
    char *sendbuf, *recvbuf;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    getopt_s(argc, argv, &size);

    sendbuf = (char*)malloc(n * size * sizeof(char));
    recvbuf = (char*)malloc(n * size * sizeof(char));

    MPI_Status status[2 * n];
    MPI_Request requests[2 * n];

    for (i = 0; i < n * size; i++) {
        sendbuf[i] = (char)(65 + ((i / size) + n * rank) % 63);
    }

    printf("[%d] (%d byte) %.*s\n", rank, n * size, size <= MAX_RPINT ? n * size : 0, sendbuf);

    start = MPI_Wtime();
    // ********************************** MPI_Alltoall **********************************

    for (i = 0; i < n; i++) {
        MPI_Isend(&sendbuf[i * size], size, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests[i]);
        MPI_Irecv(&recvbuf[i * size], size, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests[n + i]);
    }

    MPI_Waitall(2 * n, requests, status);

    // ********************************** MPI_Alltoall **********************************
    stop = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);    // Для красивого вывода
    
    if(rank == ROOT_RANK) { printf("\n"); }

    printf("[%d] (%d byte) %.*s\n", rank, n * size, size <= MAX_RPINT ? n * size : 0, recvbuf);

    MPI_Wtime_avg_comparator(stop - start, n, rank, ROOT_RANK, sendbuf, size, MPI_CHAR, recvbuf, n * size, MPI_CHAR, MPI_COMM_WORLD, MPI_Alltoall);

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}