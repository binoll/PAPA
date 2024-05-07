#include <stdio.h>
#include "mpi.h"

// MPI_Bcast с помощью неблокируемых операций (от одного ко всем)
int main(int argc, char** argv)
{
	int rank, size;
	double start_time_0, end_time_0;
	double start_time_1, end_time_1;
	int buf = 1;
	int bcast_buf;
	int temp = 0;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение размера коммуникатора
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга процесса

	MPI_Request *reqs = new MPI_Request[size-1];
	MPI_Status *stats = new MPI_Status[size-1];

	// Один процесс отправляет
	start_time_0 = MPI_Wtime();
	if (rank == 0)
	{
		bcast_buf = 0;
		for (int i = 1; i < size; i++)
		{
			MPI_Isend(&rank, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &reqs[i - 1]);
		}
		MPI_Waitall(size - 1, reqs, stats);
	}

	// Все остальные процессы получают
	else
	{
		start_time_0 = MPI_Wtime();
		MPI_Irecv(&temp, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &reqs[rank - 1]);
		MPI_Wait(&reqs[rank - 1], &stats[rank - 1]);
	}
	end_time_0 = MPI_Wtime();
	printf("\nMY: Process %d got sum %i\n", rank, temp + rank);
	if (rank == size - 1)
	{
		printf("My program: %f", end_time_0 - start_time_0); // Вывод общего времени для моей программы
	}

	start_time_1 = MPI_Wtime();
	MPI_Bcast(&bcast_buf, 1, MPI_INT, 0, MPI_COMM_WORLD); // Нормальный Bcast
	end_time_1 = MPI_Wtime();
	printf("\nHIS: Message to %d from root: %i\n", rank, bcast_buf + rank);
	if (rank == size - 1)
	{
		printf("\nHis program: %f", end_time_1 - start_time_1); // Вывод общего времени для Bcast
	}

	MPI_Finalize();
}