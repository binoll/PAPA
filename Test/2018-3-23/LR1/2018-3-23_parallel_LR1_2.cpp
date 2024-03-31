#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{

	int old_rank, new_rank, name_len, size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int color[10] = { 0, MPI_UNDEFINED, 3, 0, 3, 0, 0, 5, 3, MPI_UNDEFINED };
	int key[10] = { 3, 1, 2, 5, 1, 1, 1, 2, 1, 0 };

	MPI_Init(&argc, &argv);
	MPI_Comm new_communicator;
	MPI_Comm_rank(MPI_COMM_WORLD, &old_rank);
	MPI_Get_processor_name(processor_name, &name_len);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (old_rank == 0)
		fprintf(stdout, "Proc\tOR\tNR\tColor\tKey\n");

	MPI_Comm_split(MPI_COMM_WORLD, color[old_rank], key[old_rank], &new_communicator);

	if (color[old_rank] == MPI_UNDEFINED) 
	{
		fprintf(stderr, "Rank is undefiend. Old rank: %d\n", old_rank);
	}
	else 
	{		
		MPI_Comm_rank(new_communicator, &new_rank);
		MPI_Comm_size(new_communicator, &size);
		fprintf(stdout, "%s\t%d\t%d\t%d\t%d\n", processor_name, old_rank, new_rank, color[old_rank], key[old_rank]);
	}
	MPI_Finalize();

	return 0;
}