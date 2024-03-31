#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
    int n, rank, namelen;
    char proc_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(proc_name, &namelen);
    MPI_Finalize();
    printf("Count proc: %d, rank: %d, proc_name: %s\n", n, rank, proc_name);
}