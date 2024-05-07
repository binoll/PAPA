
/*
    1. Разработать параллельную программу, демонстрирующую применение
        глобальных операций в MPI (MPI_Reduce, MPI_Allreduce, MPI_Reduce_scatter, MPI_Scan).
    
    MPI_Reduce_scatter
*/

#include "lr3.h"

#define ROOT_RANK 0

int main(int argc, char* argv[])
{
    int rank, n, size, i;
    int *sendbuf, *recvbuf, *count;
    MPI_Op op;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    count = (int*)malloc(n * sizeof(int));

    getopt_soc(argc, argv, n, &size, &op, count);

    sendbuf = (int*)malloc(size * sizeof(int));
    recvbuf = (int*)malloc(count[rank] * sizeof(int));

    for(i = 0; i < size; i++) {
        sendbuf[i] = (size * rank + i) % 1000;
    }

    if (MAX_RPINT > size) { printf_vector(rank, size, sendbuf); }

    // ********************************** MPI_Reduce_scatter **********************************

    MPI_Reduce_scatter(sendbuf, recvbuf, count, MPI_INT, op, MPI_COMM_WORLD);
    
    // ********************************** MPI_Reduce_scatter **********************************

    MPI_Barrier(MPI_COMM_WORLD);
    
    if (rank == ROOT_RANK) { printf("\n"); }

    if (MAX_RPINT > size) { printf_vector(rank, count[rank], recvbuf); }

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}