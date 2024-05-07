#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int n, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);
    printf("All process:%d\trank:%d\tname:%s\n", n, rank, processor_name);
    MPI_Finalize();
}