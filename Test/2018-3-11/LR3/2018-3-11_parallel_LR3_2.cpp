%%writefile task2.c
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
		srand(time(NULL));
		int i, j, k, opt, n, rank, size, tag = 0;
		while ((opt = getopt(argc, argv, "s:")) != -1)
		{
			switch (opt)
			{
			case 's':
				size = atoi(optarg);
				break;
			default:
				fprintf(stderr, "Bad argument");
				exit(EXIT_FAILURE);
		}
	}
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int **a = (int **)malloc(size*sizeof(int *));
	for(int i = 0; i < size; i++)
	{
		a[i] = (int *)malloc(size*sizeof(int));
	}
	if (rank == 0)
	{
				for (i=0; i < size; i++)
				{
					for (j=0; j < size; j++)
					{
						a[i][j] = rand() % 10;
					}
				}
	}

	if (rank == 0)
	{
		printf("Rank %d send matrix:\n", rank);
		for (i = 0; i < size; i++)
		{
			for (j = 0; j < size; j++)
			{
				printf(" %d", a[i][j]);
			}
			printf ("\n");
		}
	printf ("\n\n");
	}
	MPI_Datatype ROW;
	MPI_Type_contiguous(size, MPI_INT, &ROW);
	MPI_Type_commit(&ROW);
	// передадим матрицу по строчкам всем процессам
	for (i = 0; i < size; i++)
		MPI_Bcast(&a[i][0], 1, ROW, 0, MPI_COMM_WORLD);
	if (rank == 1)
	{
		printf("Rank %d get matrix:\n", rank);
		for (i = 0; i < size; i++)
			{
				for (j = 0; j < size; j++)
				{
					printf(" %d", a[i][j]);
				}
				printf ("\n");
			}
		printf ("\n\n");
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Type_free(&ROW);
	MPI_Finalize();
	return 0;
}
