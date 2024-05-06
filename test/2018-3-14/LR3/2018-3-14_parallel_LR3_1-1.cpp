
/*
    1. Разработать параллельную программу, демонстрирующую применение
        глобальных операций в MPI (MPI_Reduce, MPI_Allreduce, MPI_Reduce_scatter, MPI_Scan).
    
    MPI_Reduce
*/

#include "lr3.h"

#define ROOT_RANK root_rank

int main(int argc, char* argv[])
{
    int rank, n, root_rank, size, i;
    int *sendbuf, *recvbuf;
    MPI_Op op;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    getopt_rso(argc, argv, n, &root_rank, &size, &op);

    sendbuf = (int*)malloc(size * sizeof(int));
    if (rank == ROOT_RANK) { recvbuf = (int*)malloc(size * sizeof(int)); }

    for(i = 0; i < size; i++) {
        sendbuf[i] = (size * rank + i) % 1000;
    }

    if (MAX_RPINT > size) { printf_vector(rank, size, sendbuf); }

    // ********************************** MPI_Reduce **********************************

    MPI_Reduce(sendbuf, recvbuf, size, MPI_INT, op, ROOT_RANK, MPI_COMM_WORLD);

    // ********************************** MPI_Reduce **********************************

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == ROOT_RANK) { 
        printf("\n");
        if (MAX_RPINT > size) { printf_vector(rank, size, recvbuf); }
    }

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}