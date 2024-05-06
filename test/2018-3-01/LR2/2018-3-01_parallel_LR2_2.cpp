#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

// Передача данных от каждого к каждому
int main(int argc, char** argv)
{
	srand(time(NULL));
	int rank, size;
	int buf = 0;
	int index;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение размера коммуникатора
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга процесса
	MPI_Request* reqs = new MPI_Request[size];
	MPI_Status* stats = new MPI_Status[size];

	// МАТРИЦА A
	int* send_buf_a = new int[size];
	for (int i = 0; i < size; i++) // Заполняем массив A для отправки
	{
		send_buf_a[i] = (rand() + rank) % 10;
	}

	for (int i = 0; i < size; i++) // Выводим матрицу A
	{
		printf("A%d:%i ", rank, send_buf_a[i]);
	}
	printf("\n");
	// МАТРИЦА B
	int* send_buf_b = new int[size];
	for (int i = 0; i < size; i++) // Заполняем массив B для отправки
	{
		send_buf_b[i] = (rand() + rank) % 10;
	}

	for (int i = 0; i < size; i++) // Выводим матрицу B
	{
		printf("B%d:%i ", rank, send_buf_b[i]);
	}
	printf("\n");

	// Транспонируем матрицу для нормального умножения
	MPI_Barrier(MPI_COMM_WORLD);
	int* send_buf_a_tr = new int[size];
	MPI_Alltoall(send_buf_a, 1, MPI_INT, send_buf_a_tr, 1, MPI_INT, MPI_COMM_WORLD);

	// Теперь каждый процесс хранит транспонированную матрицу A
	int* result = new int[size * size];
	MPI_Allgather(send_buf_a_tr, size, MPI_INT, result, size, MPI_INT, MPI_COMM_WORLD);

	for (int i = 0; i < size * size; i++)
	{
		printf("%i ", result[i]);
	}
	printf("\n");
	for (int i = 0; i < size; i++)
	{
		printf("%i ", send_buf_b[i]);
	}
	printf("\n");

	int* result_array = new int[size]; // Создали итоговый массив для каждого процесса
	for (int i = 0; i < size; i++)
	{
		int sum = 0;
		index = i * size;
		for (int j = 0; j < size; j++)
		{
			sum += send_buf_b[j] * result[index];
			printf("%i - %i. ", j, sum);
			index += 1;
		}
		printf("\n");
		result_array[i] = sum;
	}
	printf("\n");
	for (int i = 0; i < size; i++)
	{
		printf("R%d: %i ", rank, result_array[i]);
	}
	printf("\n");


	/*int* result_matrix = new int[size * size];
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Gather(result, size, MPI_INT, result_matrix, size, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0)
	{
		printf("\nResult of multiplication:\n");
		for (int i = 0; i < size * size; i++)
		{
			if (i == size)
			{
				printf("\n");
			}
			printf("%i ", result_matrix[i]);
		}
		printf("\n");
	}*/


	MPI_Finalize();
}