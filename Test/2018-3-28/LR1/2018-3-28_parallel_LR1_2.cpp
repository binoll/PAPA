#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
    int n, rank, new_rank, new_n, status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int color;
    if (rank % 3 == 0)
    {
        color = rank % 3;
    }
    else
    {
        if (rank % 4 == 0)
        {
            color = rank % 3;
        }
        else
        {
            color = MPI_UNDEFINED;
        }
    }
    MPI_Comm MPI_New_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, rank, &MPI_New_comm);
    if (MPI_New_comm == MPI_COMM_NULL)
    {
        printf("Not in new communicator\n");
    }
    else
    {
        MPI_Comm_rank(MPI_New_comm, &new_rank);
        MPI_Comm_size(MPI_New_comm, &new_n);
        printf("Old rank - %d, all proc: %d\t|  New rank - %d, all proc: %d\n", rank, n, new_rank, new_n);
    }
    MPI_Finalize();
    return 0;
}