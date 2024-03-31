#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[]){
    int old_rank, new_rank, namelen, old_size, new_size;
    int color[10] = {0, MPI_UNDEFINED, 3, 0, 3, 0, 0, 5, 3, MPI_UNDEFINED};
    int key[10] = {3, 1, 2, 5, 1, 1, 1, 2, 1, 0};
    MPI_Init(&argc, &argv);
    MPI_Comm newcomm;
    MPI_Comm_rank(MPI_COMM_WORLD, &old_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &old_size);
    MPI_Comm_split(MPI_COMM_WORLD, color[old_rank], key[old_rank], &newcomm);
    if (color[old_rank] == MPI_UNDEFINED){
	fprintf(stdout, "Process not in group. Old rank: %d\n", old_rank);
	}
    else {
	MPI_Comm_rank(newcomm, &new_rank);
	MPI_Comm_size(newcomm, &new_size);
	fprintf(stdout, "Old_rank: %d  Old_total:%d  New_rank: %d  New_total: %d\n", old_rank, old_size, new_rank, new_size);
	}
    MPI_Finalize();
    return 0;
    }