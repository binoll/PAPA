#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int proc, myrank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Get_processor_name(processor_name, &namelen);
    fprintf(stdout, "old: rank-num: %d\n", myrank);

    int colors[10] = { 0,MPI_UNDEFINED,3,0,3,0,0,5,3,MPI_UNDEFINED };
    int keys[10] = { 3,1,2,5,1,1,1,2,1,0 };

    MPI_Comm new_comm;


    MPI_Comm_split(MPI_COMM_WORLD, colors[myrank], keys[myrank], &new_comm);

    if (new_comm == MPI_COMM_NULL)
    {
        fprintf(stderr, "MPI_COMM_NULL error");
    }
    else
    {
        MPI_Comm_size(new_comm, &proc);
        MPI_Comm_rank(new_comm, &myrank);
        fprintf(stdout, "new: proc_num %d, rank: %d \n", proc, myrank);
    }

    MPI_Finalize();
}