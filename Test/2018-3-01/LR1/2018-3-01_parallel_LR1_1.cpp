#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv);
    int n, rank, name_len;
    char processor_name[30];
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &name_len);
    fprintf(stdout, "Rank: %d\nName: %s\nTotal_proc: %d\n", rank, processor_name, n);
    fprintf(stderr, "Test error from %s\n", processor_name);
    MPI_Finalize();
    }