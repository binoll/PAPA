#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[]){
    int procs, rank, namelen, new_procs, new_rank;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    MPI_Get_processor_name(processor_name, &namelen);
    
    int color[10] = {0, MPI_UNDEFINED, 3, 0, 3, 0, 0, 5, 3, MPI_UNDEFINED}; 
    int key[10] = {3, 1, 2, 5, 1, 1, 1, 2, 1, 0};

    MPI_Comm newcomm;
    MPI_Comm_split(MPI_COMM_WORLD, color[rank], key[rank], &newcomm);

    if (newcomm == MPI_COMM_NULL){
	printf("Not in color: %d\n",rank);
    }

    MPI_Comm_size(newcomm, &new_procs);
    MPI_Comm_rank(newcomm, &new_rank);
    
    printf("old_rank: %d\t new_rank: %d\t new_procs: %d\n",rank, new_rank, new_procs);
    MPI_Finalize();
}