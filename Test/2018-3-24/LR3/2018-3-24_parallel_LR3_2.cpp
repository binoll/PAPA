#include <stdio.h>  
#include <mpi.h>     
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
	srand(time(NULL));
	int opt, rank, allnum;
	int Msize = 0;
	while ((opt = getopt(argc, argv, "s:")) != -1) {
		switch (opt) {
		case 's':
			Msize = atoi(optarg);
			break;
		default:
			exit(EXIT_FAILURE);
		}
	}

	MPI_Init(&argc, &argv);
	if (MPI_Comm_rank( MPI_COMM_WORLD, &rank)) perror("Err at Comm_rank :( \n");
	if (MPI_Comm_size(MPI_COMM_WORLD, &allnum)) perror("Err at Comm_size :( \n");
	long int size_of_int;
	MPI_Datatype column, xpose;

	MPI_Type_extent(MPI_INT, &size_of_int);
	MPI_Type_vector(Msize, 1, Msize, MPI_INT, &column);
	MPI_Type_hvector(Msize, 1, size_of_int, column, &xpose);
	MPI_Type_commit(&xpose);
	if (rank == 0)
	{
		int a[Msize][Msize];
		printf("\n Matrix a in rank %d:\n", rank);
		for (int i = 0; i < Msize; ++i)
		{
			for (int j = 0; j < Msize; ++j)
			{
				a[i][j] = rand() % 10;
				printf(" %d ", a[i][j]);
			}
			printf("\n");
		}
		MPI_Bcast(a, 1, xpose, 0, MPI_COMM_WORLD);
	}
	else
	{
		int b[Msize][Msize];
		MPI_Bcast(b, 1, xpose, 0, MPI_COMM_WORLD);
		printf("\n Matrix a in rank %d:\n", rank);
		for (int i = 0; i < Msize; ++i)
		{
			for (int j = 0; j < Msize; ++j) printf(" %d ", b[i][j]);
			printf("\n");
		}
	}
	MPI_Finalize();
	return 0;
}