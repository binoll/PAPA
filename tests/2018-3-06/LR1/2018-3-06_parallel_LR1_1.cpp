#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]){

    int myrank, namelen, n, a;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Get_processor_name(processor_name, &namelen);
    if (myrank == 0) {
    printf("Not now, my friend\n rank=0\n");
    }
    else {
	printf("\nMy rank is %d \n", myrank);
	printf("Name VM is %s \n", processor_name);
    }
    MPI_Finalize();
    }
