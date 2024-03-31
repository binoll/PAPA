#include <stdio.h>
#include "mpi.h"

// MPI_Barrier с помощью неблокируемых операций
int main(int argc, char** argv)
{
	int rank, size;
	double start_time_0, end_time_0;
	double start_time_1, end_time_1;
	int buf = 0;
	int index;
	int temp;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение размера коммуникатора
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга процесса
	MPI_Request* reqs = new MPI_Request[size * 2];
	MPI_Status* stats = new MPI_Status[size * 2];
	MPI_Request request[2];
	MPI_Status status[2];

	// Все отправляют и все принимают
	start_time_0 = MPI_Wtime();
	for (int i = 0; i < size; i++)
	{
		MPI_Isend(&rank, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &request[0]);
		MPI_Irecv(&temp, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &request[1]);
		MPI_Waitall(2, request, status); // Почти MPI_Barrier
		buf += temp;
	}
	end_time_0 = MPI_Wtime();
	printf("MY: Process %i got sum %i\n", rank, buf);
	if (rank == size - 1)
	{
		printf("\nMy program: %f\n\n", end_time_0 - start_time_0); // Вывод общего времени для моей программы
	}

	// Все отправляют и все принимают
	buf = 0;
	start_time_1 = MPI_Wtime();
	for (int i = 0; i < size; i++)
	{
		MPI_Isend(&rank, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &reqs[i]);
		MPI_Irecv(&temp, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &reqs[i + size]);
		MPI_Barrier(MPI_COMM_WORLD);
		buf += temp;
	}
	end_time_1 = MPI_Wtime();
	printf("BARRIER: Process %i got sum %i\n", rank, buf);
	if (rank == 0)
	{
		printf("\nHis program: %f\n\n", end_time_1 - start_time_1); // Вывод общего времени для Bcast
	}

	MPI_Finalize();
}