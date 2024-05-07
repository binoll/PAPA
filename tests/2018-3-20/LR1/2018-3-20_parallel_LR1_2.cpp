#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size, namelen;
    char proc_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int color[10] = { 0, MPI_UNDEFINED, 3, 0, 3, 0, 0, 5, 3, MPI_UNDEFINED };
    int key[10] = { 3, 1, 2, 5, 1, 1, 1, 2, 1, 0 };

    MPI_Comm new_comm;


    MPI_Comm_split(MPI_COMM_WORLD, color[rank], key[rank], &new_comm);

    if (new_comm == MPI_COMM_NULL) {
        printf("Not in color: %d\n", rank);
    }

    int new_size;
    int new_rank;
    MPI_Comm_size(new_comm, &new_size);
    MPI_Comm_rank(new_comm, &new_rank);

    printf("Old Rank: %d, Size: %d, New Rank: %d, Color: %d\n", rank, new_size, new_rank, color[rank]);

    
    MPI_Finalize();
    
}