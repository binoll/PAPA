#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int n, rank, namelen, n_new, rank_new;
    //char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //MPI_Get_processor_name(processor_name, &namelen);
    int color = 0;
    if (rank==3 || rank==7 || rank==5)
    {
	color = MPI_UNDEFINED;
    }
    MPI_Comm MPI_NEW_COMM;
    MPI_Comm_split(MPI_COMM_WORLD, color, rank, &MPI_NEW_COMM);
    if (MPI_NEW_COMM == MPI_COMM_NULL)
    {
	printf("Process with rank %d do not enter in new comm\n", rank);
	return 0;
    }
    MPI_Comm_size(MPI_NEW_COMM, &n_new);
    MPI_Comm_rank(MPI_NEW_COMM, &rank_new);
    printf("old com rank:%d\tnew com rank:%d\tall process new com:%d\n", rank, rank_new, n_new);
    MPI_Finalize();
}