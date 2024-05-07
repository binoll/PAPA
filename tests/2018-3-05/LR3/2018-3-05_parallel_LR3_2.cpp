#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "windows.h"
#define MSMPI_NO_DEPRECATE_20

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);

	int myrank, process_count = 0;

	MPI_Comm_size(MPI_COMM_WORLD, &process_count);
   
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	srand(time(NULL));

	int matrix_size = atoi(argv[1]);

	MPI_Datatype column, xpose;
	int sizeofint;

	MPI_Type_vector(matrix_size, matrix_size, matrix_size, MPI_INT, &column);
	MPI_Type_commit(&column);

	int** matrix = new int* [matrix_size];
	for (int i = 0; i < matrix_size; i++) matrix[i] = new int[matrix_size];

	if (myrank == 0)
	{
		for (int i = 0; i < matrix_size; i++)
		{
			printf("Process with rank 0, row %d: ", i + 1);
			for (int j = 0; j < matrix_size; j++)
			{
				matrix[i][j] = rand();
				printf("%d ", matrix[i][j]);
			}
			printf("\n");
		}

		printf("\n");
	}

	MPI_Bcast(matrix, 1, column, 0, MPI_COMM_WORLD);

	int** recv_matrix = new int* [matrix_size];
	for (int i = 0; i < matrix_size; i++) recv_matrix[i] = new int[matrix_size];

	for (int i = 0; i < matrix_size; i++)
	{
		printf("Process with rank %d, get row %d: ", myrank, i + 1);
		for (int j = 0; j < matrix_size; j++)
		{
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}

	printf("\n");

	MPI_Finalize();

	return 0;

}
