#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

// Редукция
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

	int* recv_buf_root = new int[size]; // Создаем массив для приема сообщений в root

	MPI_Reduce(send_buf, recv_buf_root, size, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // Редукция с операцией суммирования

	// Выводим полученный результат суммирования
	if (rank == 0)
	{
		printf("Root got sum: ");
		for (int i = 0; i < size; i++)
		{
			printf("%i ", recv_buf_root[i]);
		}
	}

	MPI_Finalize();
}