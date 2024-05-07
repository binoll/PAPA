#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"
#include <stdio.h>
#include <cstdlib>
#include <algorithm>
using namespace std;

// Alltoall с помощью неблокируемых операций (от каждого к каждому - транспонирование матрицы)
int main(int argc, char** argv)
{
	srand(time(NULL));
	int rank, size;
	int buf = 0;
	int index;
	double start_time_0, end_time_0;
	double start_time_1, end_time_1;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение размера коммуникатора
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга процесса
	MPI_Request* reqs = new MPI_Request[size * size * 2];
	MPI_Status* stats = new MPI_Status[size * size * 2];

	int* send_buf = new int[size]; // Создание массива отправки для каждого процесса
	for (int i = 0; i < size; i++)
	{
		send_buf[i] = rank;  // Заполняем массив
	}

	printf("Initial process message %d: ", rank); // Вывод начального массива
	for (int i = 0; i < size; i++)
	{
		printf("%i ", send_buf[i]);
	}
	printf("\n");

	int* recv_buf = new int[size]; // Создание массива для приема сообщений (матрица)

	int count = 0;
	start_time_0 = MPI_Wtime();
	for (int i = 0; i < size; i++) // Для каждого элемента массива у процесса
	{
		MPI_Isend(&send_buf[i], 1, MPI_INT, i, 5, MPI_COMM_WORLD, &reqs[rank + i]);
		MPI_Irecv(&recv_buf[i], 1, MPI_INT, i, 5, MPI_COMM_WORLD, &reqs[rank + i + size]);
		MPI_Wait(&reqs[rank + i + size], &stats[rank + i + size]);
	}
	end_time_0 = MPI_Wtime();

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

	printf("CUSTOM: Process %d got message: ", rank); // Вывод резалтика
	for (int i = 0; i < size; i++)
	{
		printf("%i ", recv_buf[i]);
	}
	printf("\n");
	int* recv_buf1 = new int[size];
	start_time_1 = MPI_Wtime();
	MPI_Alltoall(send_buf, 1, MPI_INT, recv_buf1, 1, MPI_INT, MPI_COMM_WORLD);
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

	printf("ALL: Process %d got message: ", rank);
	for (int i = 0; i < size; i++)
	{
		printf("%i ", recv_buf1[i]);
	}
	printf("\n");
	MPI_Finalize();
}