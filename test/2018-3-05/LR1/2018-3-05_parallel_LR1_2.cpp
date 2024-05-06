#include "mpi.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);

	int myrank, namelen, process_count;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Comm_size(MPI_COMM_WORLD, &process_count);

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	MPI_Get_processor_name(processor_name, &namelen);

	int color[] = {0, 1, 0, 0, 4, MPI_UNDEFINED, 4, 1, 0, MPI_UNDEFINED};

	MPI_Comm newcomm;
	MPI_Comm_split(MPI_COMM_WORLD, color[myrank], 1, &newcomm);

	if (newcomm != MPI_COMM_NULL)
	{
		int newmyrank, new_process_count;

		MPI_Comm_size(newcomm, &new_process_count);

		MPI_Comm_rank(newcomm, &newmyrank);

		printf("Old process quantity: %d, New process quantity: %d\n", process_count, new_process_count);
		printf("Old process rank: %d, New process rank: %d\n", myrank, newmyrank);
		printf("\n");
	}
	else {
		printf("Old process quantity: %d, New process quantity: ERROR\n", process_count);
		printf("Old process rank: %d, New process rank: ERROR\n", myrank);
		printf("\n");
	}

	MPI_Finalize();

}
