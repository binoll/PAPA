#include <stdio.h>
#include "mpi.h"

// Передача данных от каждого к каждому
int main(int argc, char** argv)
{
	int rank, size;
	int buf = 0;
	int index;
	MPI_Status stats[2];

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение размера коммуникатора
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга процесса
	MPI_Request reqs[2];

	for (int i = 0; i < size; i++)
	{
		if (i != rank)
		{
			int temp;
			MPI_Isend(&rank, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &reqs[0]);
			MPI_Irecv(&temp, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &reqs[1]);
			MPI_Waitall(2, reqs, stats);
			buf += temp;
		}
	}
	printf("Process %i got sum %i\n", rank, buf);
	MPI_Finalize();
}