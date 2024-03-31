#include <stdio.h>
#include "mpi.h"

// ѕередача данных по кольцу методом Ё—“ј‘≈“Ќјя ѕјЋќ„ ј
int main(int argc, char** argv)
{
	int rank, size, prev, next;
	int buf;
	MPI_Request reqs[2];
	MPI_Status stats[2];

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size); // ќпределение размера коммуникатора
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // ќпределение ранга процесса

	next = rank + 1;
	prev = rank - 1;
	if (rank == 0) prev = size - 1;
	if (rank == size - 1) next = 0;

	if (rank == 0)
	{
		MPI_Isend(&rank, 1, MPI_INT, next, 6, MPI_COMM_WORLD, &reqs[1]);
		MPI_Waitall(1, reqs, stats);
		printf("process %d sended message to %i: %i \n\n", rank, next, rank);
		MPI_Irecv(&buf, 1, MPI_INT, prev, 6, MPI_COMM_WORLD, &reqs[0]);
		MPI_Waitall(1, reqs, stats);
		printf("process %d got message from %i: %i \n", rank, prev, buf);
	}

	else
	{
		MPI_Irecv(&buf, 1, MPI_INT, prev, 6, MPI_COMM_WORLD, &reqs[0]);
		MPI_Waitall(1, reqs, stats);
		buf += 1;
		printf("process %d got message from %i: %i \n", rank, prev, buf);
		MPI_Isend(&buf, 1, MPI_INT, next, 6, MPI_COMM_WORLD, &reqs[1]);
		MPI_Waitall(1, reqs, stats);
		printf("process %d sended message to %i: %i \n\n", rank, next, buf);
	}

	MPI_Finalize();
}