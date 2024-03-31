/*
Разработать параллельную программу, осуществляющую 
разделение коммуникатора из лекционного примера.
 */

#include <stdio.h>
#include "mpi.h"

int main(int argc, char** argv){
    int old_rank, name_len, new_size, new_rank, old_size;
    MPI_Init(&argc, &argv);
    MPI_Comm new_comm;
    MPI_Comm_rank(MPI_COMM_WORLD, &old_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &old_size);
    int colors[] = {0, MPI_UNDEFINED, 3, 0, 3, 0, 0, 5, 3, MPI_UNDEFINED};
    int keys[] = {3, 1, 2, 5, 1, 1, 1, 2, 1, 0};
    MPI_Comm_split(MPI_COMM_WORLD, colors[old_rank], keys[old_rank], &new_comm);
    if (colors[old_rank] == MPI_UNDEFINED){
	fprintf(stdout, "Old rank: %d, no new comm\n", old_rank);
    } else {
	MPI_Comm_rank(new_comm, &new_rank);
	MPI_Comm_size(new_comm, &new_size);
	fprintf(stdout, "Old rank: %d   Old size: %d   New rank: %d   New comm size: %d\n", old_rank,old_size, new_rank, new_size);
	
    }
    MPI_Finalize();
}
