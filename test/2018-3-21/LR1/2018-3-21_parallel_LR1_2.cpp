#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

int main(int argc, char* argv[])
{
	int n, rank, namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	MPI_Init(&argc, &argv);
	MPI_Comm new_comm;
	
	int color[] = {0, MPI_UNDEFINED, 3, 0, 3, 0, 0, 5, 3, MPI_UNDEFINED};
	int key[] = { 3, 1, 2, 5, 1, 1, 1, 2, 1, 0 };
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

	MPI_Comm_split(MPI_COMM_WORLD, color[rank], key[rank], &new_comm);
	MPI_Comm_size(new_comm, &n);

	if (rank == 0) {
		printf("node name — %s, rank — %d, size — %d \n", processor_name, rank, n);
	}
	else {
		printf("node name — %s, rank — %d, size — %d \n", processor_name, rank, n);
	}
	MPI_Finalize();
	return 0;
}


