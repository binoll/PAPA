#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"


int main(int argc, char** argv)
{
	srand(time(NULL));
	int rank, size;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение размера коммуникатора
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга процесса
	MPI_Request* reqs = new MPI_Request[size];
	MPI_Status* stats = new MPI_Status[size];

	// Заполняем массив для каждого процесса
	int* send_buf = new int[size];
	for (int i = 0; i < size; i++)
	{
		send_buf[i] = (rand() + rank) % 10;
	}
	// Выводим начальный массив
	for (int i = 0; i < size; i++)
	{
		printf("%d:%i ", rank, send_buf[i]);
	}
	printf("\n");

	int* count = new int[size];
	// Заполняем count случайными значениями в root
	if (rank == 0)
	{
		int remain = size;
		for (int i = 0; i < size; i++)
		{
			if (i != size - 1)
			{
				count[i] = rand() % remain;
				remain -= count[i];
			}
			else
			{
				count[i] = remain;
			}
		}
		for (int i = 0; i < size; i++) // Выводим count
		{
			printf("c:%i ", count[i]);
		}
		printf("\n");
	}
	MPI_Bcast(count, size, MPI_INT, 0, MPI_COMM_WORLD); // Передаем count всем процессам от root

	// Массив для приема итоговых значений
	int* recv_buf = new int[size];
	for (int i = 0; i < size; i++)
	{
		recv_buf[i] = -1;
	}


	MPI_Reduce_scatter(send_buf, recv_buf, count, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

	for (int i = 0; i < size; i++)
	{
		printf("Res %d:%i ", rank, recv_buf[i]);
	}
	printf("\n");

	MPI_Finalize();
}