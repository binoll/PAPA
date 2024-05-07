#include <stdio.h>
#include <cstdlib>
#include <algorithm>
#include "mpi.h"
using namespace std;

// Allgather с помощью неблокируемых операций (от каждого к каждому)
int main(int argc, char** argv)
{
	int rank, size;
	int buf = 0;
	int index;
	double start_time_0, end_time_0;
	double start_time_1, end_time_1;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение размера коммуникатора
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга процесса
	MPI_Request* reqs = new MPI_Request[size * 2];
	MPI_Status* stats = new MPI_Status[size * 2];

	int* recv_buf = new int[size];

	int* send_buf = new int[size];
	start_time_0 = MPI_Wtime();
	for (int i = 0; i < size; i++)
	{
		MPI_Isend(&rank, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &reqs[i + size]);
		MPI_Irecv(&recv_buf[i], 1, MPI_INT, i, 5, MPI_COMM_WORLD, &reqs[i]);
	}
	MPI_Waitall(size * 2, reqs, stats);
	end_time_0 = MPI_Wtime();
	

	// Ищем максимальное время работы программы
	double time = end_time_0 - start_time_0;
	MPI_Isend(&time, 1, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD, &reqs[rank + size]);
	if (rank == 0)
	{
		double* total_time = new double[size];
		for (int i = 0; i < size; i++)
		{
			MPI_Irecv(&total_time[i], 1, MPI_DOUBLE, i, 5, MPI_COMM_WORLD, &reqs[i]);
			MPI_Wait(&reqs[i], &stats[i]);
			MPI_Wait(&reqs[rank + size], &stats[rank + size]);

		}
		printf("\n");
		// Вывод общего времени для программы с MPI_Scatter (используем функцию для поиска max элемента из массива времен)
		printf("MY program: %f\n", *max_element(total_time, total_time + size));
	}

	printf("Proccess %d got message: ", rank);
	for (int i = 0; i < size; i++)
	{
		printf("%i ", recv_buf[i]);
	}

	// Использование Allgather
	int* recv_buf_1 = new int[size];
	start_time_1 = MPI_Wtime();
	MPI_Allgather(&rank, 1, MPI_INT, recv_buf_1, 1, MPI_INT, MPI_COMM_WORLD);
	end_time_1 = MPI_Wtime();

	// Ищем максимальное время работы программы
	double time1 = end_time_1 - start_time_1;
	MPI_Isend(&time1, 1, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD, &reqs[rank + size]);
	if (rank == 0)
	{
		double* total_time = new double[size];
		for (int i = 0; i < size; i++)
		{
			MPI_Irecv(&total_time[i], 1, MPI_DOUBLE, i, 5, MPI_COMM_WORLD, &reqs[i]);
			MPI_Wait(&reqs[i], &stats[i]);
			MPI_Wait(&reqs[rank + size], &stats[rank + size]);

		}
		printf("\n");
		// Вывод общего времени для программы с MPI_Scatter (используем функцию для поиска max элемента из массива времен)
		printf("HIS program: %f\n", *max_element(total_time, total_time + size));
	}

	MPI_Finalize();
}