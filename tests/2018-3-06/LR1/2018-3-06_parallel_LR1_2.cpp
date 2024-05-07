#include "mpi.h"
#include <stdio.h>

int main(int argc, char** argv) {
	int rank, size, subCommIndex, subRank, subSize;
	MPI_Comm subComm;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int colors[] = { 0,MPI_UNDEFINED,3,0,3,0,0,5,3,MPI_UNDEFINED};
	int keys[] = { 3,1,2,5,1,1,1,2,1,0 };
	MPI_Comm_split(MPI_COMM_WORLD, colors[rank], keys[rank], &subComm);

	if (subComm == MPI_COMM_NULL)
		printf("I'm not attached to any sub-communicator\n");
	else {
		MPI_Comm_size(subComm, &subSize);
		MPI_Comm_rank(subComm, &subRank);
		printf("My old rank = %d, old size = %d\n My new rank = %d, new size = %d\n", rank, size, subRank, subSize);
	}
	MPI_Comm_free(&subComm);
	MPI_Finalize();
}
