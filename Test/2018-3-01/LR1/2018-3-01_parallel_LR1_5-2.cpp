#include <stdio.h>
#include "mpi.h"

// Master - slave
int main(int argc, char** argv)
{
	int rank, size;
	int buf = 0;
	MPI_Status stats[2];

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение размера коммуникатора
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга процесса

	MPI_Request reqs[1];

	if (rank == 0)
	{
		for (int i = 1; i < size; i++)
		{
			int temp;
			MPI_Irecv(&temp, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &reqs[0]);
			MPI_Waitall(1, reqs, stats);
			buf += temp;
		}
		printf("\nSum of all ranks = %i\n", buf);
		for (int i = 1; i < size; i++)
		{
			MPI_Isend(&buf, 2, MPI_INT, i, 5, MPI_COMM_WORLD, &reqs[0]);
			MPI_Waitall(1, reqs, stats);
		}
		
	}

	else
	{
		int res;
		MPI_Isend(&rank, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &reqs[0]);
		MPI_Waitall(1, reqs, stats);
		MPI_Irecv(&res, 2, MPI_INT, 0, 5, MPI_COMM_WORLD, &reqs[0]);
		MPI_Waitall(1, reqs, stats);
		printf("Process %i got message from rank0: %i\n", rank, res);
	}

	MPI_Finalize();
}