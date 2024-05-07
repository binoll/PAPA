/*
Разработать параллельную программу, демонстрирующую использование производных типов
данных совместно с операциями коллективного взаимодействия на примере квадратной
матрицы произвольного размера. Размер квадратной матрицы должен задаваться в
качестве входного параметра. Элементы матрицы должны генерироваться случайным образом.
*/

#include <stdio.h>   // printf()
#include <mpi.h>     // MPI functions
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
	srand(time(NULL));
	int opt;
	int size_mtr = 0;
	while ((opt = getopt(argc, argv, "n:")) != -1) {
		switch (opt) {
		case 'n':
			size_mtr = atoi(optarg);
			break;
		default:
			exit(EXIT_FAILURE);
		}
	}

	MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int size_of_int;
	MPI_Datatype column, xpose;

	MPI_Type_extent(MPI_INT, &size_of_int);
	MPI_Type_vector(size_mtr, 1, size_mtr, MPI_INT, &column);
	MPI_Type_hvector(size_mtr, 1, size_of_int, column, &xpose);
	MPI_Type_commit(&xpose);
	if (rank == 0)
	{
		int a[size_mtr][size_mtr];
		int i = 0;
		while (i < size_mtr)
		{
			int j = 0;
			while (j < size_mtr)
			{
				a[i][j] = rand() % 10;
				j++;
			}
			i++;
		}
		i = 0;
		printf("RANK: %d\n", rank);
		while (i < size_mtr)
		{
			int j = 0;
			while (j < size_mtr)
			{
				printf("%d ", a[i][j]);
				j++;
			}
			printf("\n");
			i++;
		}		
		MPI_Bcast(a, 1, xpose, 0, MPI_COMM_WORLD);
	}
	else
	{
		int b[size_mtr][size_mtr];
		MPI_Bcast(b, 1, xpose, 0, MPI_COMM_WORLD);
		int i = 0;
		printf("RANK: %d\n", rank);
		while (i < size_mtr)
		{
			int j = 0;
			while (j < size_mtr)
			{
				printf("%d ", b[i][j]);
				j++;
			}
			printf("\n");
			i++;
		}
		
	}
	MPI_Finalize();
	return 0;
}