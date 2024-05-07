#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	int n, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int colors[10] = { 0, MPI_UNDEFINED, 3, 0, 3, 0, 0, 5, 3, MPI_UNDEFINED };
	int keys[10] = { 3, 1, 2, 5, 1, 1, 1, 2, 1, 0 };

	MPI_Comm newcomm;
	MPI_Comm_split(MPI_COMM_WORLD, colors[rank], keys[rank], &newcomm);
	if (newcomm == MPI_COMM_NULL) {
		printf("Process in old comm. ");
		printf("Rank: %i\n", rank);
	}
	else {
		printf("Number of processes: %i. Rank: %i\n", n, rank);
	}
	MPI_Finalize();
}