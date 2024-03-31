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

	if (myrank == 0) {
		printf("Process quantity: %d\n", process_count);
		printf("Process rank: %d\n", myrank);
		printf("Name CM: %s\n", processor_name);
		printf("\n");
	}
	else {
		printf("Process quantity: %d\n", process_count);
		printf("Process rank: %d\n", myrank);
		printf("Name CM: %s\n", processor_name);
		printf("\n");
	}

	MPI_Finalize();

}
