#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int n, rank, namelen, old_rank;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    old_rank = rank;
    MPI_Get_processor_name(processor_name, &namelen);
    printf("rank is %d, proc_name is %s\n", rank, processor_name);
    
    int key[10] = {3,1,2,5,1,1,1,2,1,0};
    int color[10] = {0,MPI_UNDEFINED,3,0,3,0,0,5,3,MPI_UNDEFINED};
    MPI_Comm new_comm;
    
    MPI_Comm_split(MPI_COMM_WORLD, color[rank], key[rank], &new_comm);
    
    if (new_comm == MPI_COMM_NULL)
        printf("old_rank %d, color %d, key %d\n", old_rank, color[old_rank], key[old_rank]);

    else
    {
        MPI_Comm_size(new_comm, &n);
        MPI_Comm_rank(new_comm, &rank);
        printf("old_rank %d, color %d, key %d, new_rank %d\n", old_rank, color[old_rank], key[old_rank], rank);
    }
    MPI_Finalize();
}