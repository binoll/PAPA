
/*
    1. Разработать параллельную программу, демонстрирующую применение
        глобальных операций в MPI (MPI_Reduce, MPI_Allreduce, MPI_Reduce_scatter, MPI_Scan).
    
    MPI_Scan
*/

#include "lr3.h"

#define ROOT_RANK 0

int main(int argc, char* argv[])
{
    int rank, n, size, i;
    int *sendbuf, *recvbuf;
    MPI_Op op;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    getopt_so(argc, argv, n, &size, &op);

    sendbuf = (int*)malloc(size * sizeof(int));
    recvbuf = (int*)malloc(size * sizeof(int));

    for(i = 0; i < size; i++) {
        sendbuf[i] = (size * rank + i) % 1000;
    }

    if (MAX_RPINT > size) { printf_vector(rank, size, sendbuf); }

    // ********************************** MPI_Scan **********************************

    MPI_Scan(sendbuf, recvbuf, size, MPI_INT, op, MPI_COMM_WORLD);

    // ********************************** MPI_Scan **********************************

    MPI_Barrier(MPI_COMM_WORLD);
    
    if(rank == ROOT_RANK) { printf("\n"); }

    if (MAX_RPINT > size) { printf_vector(rank, size, recvbuf); }

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}