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

	int* recv_buf = new int[size]; // Создаем массив для приема сообщений для каждого процесса

	MPI_Allreduce(send_buf, recv_buf, size, MPI_INT, MPI_MAX, MPI_COMM_WORLD); // Передача максимального элемента в каждом из столбцов

	// Выводим полученный результат суммирования
	
	printf("Process %d got max: ", rank);
	for (int i = 0; i < size; i++)
	{
		printf("%i ", recv_buf[i]);
	}
	printf("\n");

	MPI_Finalize();
}