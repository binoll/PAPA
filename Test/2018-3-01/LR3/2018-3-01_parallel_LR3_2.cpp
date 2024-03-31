#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>
#include "mpi.h"

// Транспонирование матрицы через производные типы данных и операции коллективного взаимодействия
int main(int argc, char** argv)
{
	srand(time(NULL));
	int rank, size;
	int MATR_SIZE;
	int opt;
	
	while ((opt = getopt(argc, argv, "s:")) != -1)
	{
		switch(opt)
		{
			case 's':
			{
				MATR_SIZE = atoi(optarg); // Размер матрицы
				break;
			}
			default:
			fprintf(stderr, "Использование: %s [-n process_id]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size); // Определение размера коммуникатора
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определение ранга процесса

	// Проверка на то, что число процессов не превышает размер матрицы
	if (size > MATR_SIZE)
	{
		if (rank == 0)
		{
			printf("Process number must be <= size of matrix");
		}
		MPI_Barrier(MPI_COMM_WORLD);
		return 0;
	}

	int* row_init = new int[MATR_SIZE]; // Каждый процесс будет заполнять свою строку

	// Распределяем строки матрицы равномерно между процессами
	int* count = new int[size];
	if (rank == 0)
	{
		for (int i = 0; i < size; i++)
		{
			count[i] = int(MATR_SIZE / size);
		}
		for (int i = 0; i < MATR_SIZE % size; i++)
		{
			count[i] += 1;
		}
	}
	MPI_Bcast(count, size, MPI_INT, 0, MPI_COMM_WORLD);
	if (rank == 0)
	{
		printf("Count: ");
		for (int i = 0; i < size; i++)
		{
			printf("%i ", count[i]);
		}
		printf("\n");
	}
	int num_rows = count[rank];

	// У каждого процесса создаем матрицу и заполняем строки в соответствии с count
	int** send_buf = new int* [MATR_SIZE];
	int* send_buf_array = new int[MATR_SIZE * MATR_SIZE];
	for (int i = 0; i < MATR_SIZE; i++)
	{
		send_buf[i] = new int[MATR_SIZE];
	}
	int counter = 0;
	for (int i = 0; i < num_rows; i++)
	{
		for (int j = 0; j < MATR_SIZE; j++)
		{
			send_buf[i][j] = (rand() + rank) % 10 + 1;
			send_buf_array[counter] = send_buf[i][j];
			counter += 1;
		}
	}
	for (int i = 0; i < MATR_SIZE * num_rows; i++)
	{
		if (i == MATR_SIZE)
		{
			printf("\n");
		}
		printf("%d:%i ", rank, send_buf_array[i]);
	}
	printf("\n");
	
	// Собираем весь мусор в root
	int* recv_buf = new int[MATR_SIZE * MATR_SIZE * size];
	MPI_Gather(send_buf_array, MATR_SIZE * MATR_SIZE, MPI_INT, recv_buf, MATR_SIZE * MATR_SIZE, MPI_INT, 0, MPI_COMM_WORLD);

	// Делаем полезную работу в root
	if (rank == 0)
	{
		// Делаем нормальную матрицу
		int** matrix = new int* [MATR_SIZE];
		for (int i = 0; i < MATR_SIZE; i++)
		{
			matrix[i] = new int[MATR_SIZE];
		}
		int* temp_array = new int[MATR_SIZE * MATR_SIZE];
		int counter_matr = 0;
		for (int i = 0; i < MATR_SIZE * MATR_SIZE * size; i++)
		{
			if (recv_buf[i] > 0 and recv_buf[i] < 11)
			{
				temp_array[counter_matr] = recv_buf[i];
				counter_matr += 1;
			}
		}
		int row = 0;
		int col = 0;
		for (int i = 0; i < MATR_SIZE * MATR_SIZE; i++)
		{
			if (i % MATR_SIZE == 0 and i != 0)
			{
				row += 1;
				col = 0;
			}
			matrix[row][col] = temp_array[i];
			col += 1;
		}

		// Транспонируем матрицу с помощью производных типов данных
		int* matrix_tr = (int*)malloc(MATR_SIZE * MATR_SIZE * sizeof(int));
		int* matrix_test = (int*)malloc(MATR_SIZE * MATR_SIZE * sizeof(int));
		printf("\nMATRIX_TEST\n");
		for (int i = 0; i < MATR_SIZE; i++)
		{
			for (int j = 0; j < MATR_SIZE; j++)
			{
				*(matrix_test + j * MATR_SIZE + i) = matrix[i][j];
			}
		}
		for (int i = 0; i < MATR_SIZE; i++)
		{
			for (int j = 0; j < MATR_SIZE; j++)
			{
				printf("%i ", *(matrix_test + j * MATR_SIZE + i));
			}
			printf("\n");
		}
		printf("\n");
		int myrank;
		long int sizeofint;
		MPI_Datatype column, xpose;
		MPI_Status status;
		MPI_Type_extent(MPI_INT, &sizeofint);
		// count | blocklength | stride
		MPI_Type_vector(MATR_SIZE, 1, MATR_SIZE, MPI_INT, &column);
		MPI_Type_hvector(MATR_SIZE, 1, sizeofint, column, &xpose);
		MPI_Type_commit(&xpose);
		MPI_Sendrecv(matrix_test, 1, xpose, rank, 0, matrix_tr, MATR_SIZE * MATR_SIZE, MPI_INT, rank, 0, MPI_COMM_WORLD, &status);

		printf("\nMATRIX_TRANSPARENT\n");
		for (int i = 0; i < MATR_SIZE; i++)
		{
			for (int j = 0; j < MATR_SIZE; j++)
			{
				printf("%i ", *(matrix_tr + j * MATR_SIZE + i));
			}
			printf("\n");
		}
		printf("\n");
		MPI_Type_free(&xpose);
	}

	MPI_Finalize();
}