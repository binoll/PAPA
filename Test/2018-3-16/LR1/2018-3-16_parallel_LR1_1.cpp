#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>


int main(int argc, char* argv[]) {
    int myrank, namelen, proc;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Get_processor_name(processor_name, &namelen);
    MPI_Comm_size(MPI_COMM_WORLD, &proc);
    printf("Hi my rank is %d, Processor name: %s, processor count=%d\n", myrank, processor_name, proc);


    MPI_Finalize();
}



