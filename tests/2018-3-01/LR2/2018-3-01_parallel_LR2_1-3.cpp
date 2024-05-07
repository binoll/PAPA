#include <stdio.h>
#include "mpi.h"

// MPI_Gather с помощью неблокируемых операций (от всех к одному)
int main(int argc, char** argv)
{
	int rank, size;
	int buf, sum = 0;
	double start_time_0, end_time_0;
	double start_time_1, end_time_1;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение размера коммуникатора
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга процесса

	MPI_Request* reqs = new MPI_Request[size - 1];
	MPI_Status* stats = new MPI_Status[size - 1];

	// Один процесс получает
	start_time_0 = MPI_Wtime();
	if (rank == 0)
	{
		for (int i = 1; i < size; i++)
		{
			MPI_Irecv(&buf, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &reqs[i-1]);
			MPI_Wait(&reqs[i - 1], &stats[i - 1]);
			printf("Proccess %d got message from %i: %i\n", rank, i, buf);
		} 
	}

	// Все остальные процессы отправляют
	else
	{
		MPI_Isend(&rank, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &reqs[rank - 1]);
		MPI_Wait(&reqs[rank - 1], &stats[rank - 1]);
	}
	end_time_0 = MPI_Wtime();
	if (rank == 0)
	{
		printf("MY program: %f", end_time_0 - start_time_0); // Вывод общего времени для моей программы
	}
	int* result_array = new int[size];
	start_time_1 = MPI_Wtime();
	MPI_Gather(&rank, 1, MPI_INT, result_array, 1, MPI_INT, 0, MPI_COMM_WORLD);
	end_time_1 = MPI_Wtime();
	if (rank == 0)
	{
		for (int i = 0; i < size; i++)
		{
			printf("%i ", result_array[i]);
		}
		printf("\nHIS program: %f\n", end_time_1 - start_time_1);
	}

	MPI_Finalize();
}