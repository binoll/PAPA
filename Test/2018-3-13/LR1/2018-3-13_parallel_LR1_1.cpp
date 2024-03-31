#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[]){
    int procs, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    MPI_Get_processor_name(processor_name, &namelen);
    
    printf("rank: %d\t procs: %d\t namelen: %s\n",rank, procs,processor_name );
    MPI_Finalize();
}