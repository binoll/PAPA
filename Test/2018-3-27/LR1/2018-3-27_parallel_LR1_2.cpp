#include <stdio.h>

#include "mpi.h"

int main(int argc, char** argv) 
{
	int oldrank, namelen, newsize, newrank;
	// Задание разделения коммуникационной среды по лекции
	int colors[] = {0,MPI_UNDEFINED,3,0,3,0,0,5,3,MPI_UNDEFINED};
	int keys[] = {3,1,2,5,1,1,1,2,1,0};
	int size;
	

	
	MPI_Comm newcomm;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &oldrank);
	
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	if (size != 10)
	{
		fprintf(stderr, "NP must be equal to 10!\n");
		MPI_Abort(MPI_COMM_WORLD, 0);
	}
	
	if (oldrank == 0)
	{
		printf("MPI_UNDEFINED: %d\n", MPI_UNDEFINED);
	}
	
	// Разделение коммуникационной среды
	MPI_Comm_split(MPI_COMM_WORLD, colors[oldrank], keys[oldrank], &newcomm);
	if (colors[oldrank] == MPI_UNDEFINED)
	{
		printf("old rank: %d, no new comm. colour: %d\n", oldrank, colors[oldrank]);
	}
	else
	{
		MPI_Comm_rank(newcomm, &newrank);
		MPI_Comm_size(newcomm, &newsize);
		printf("old: %d, new: %d, new comm size: %d. colour old: %d, new: %d\n", oldrank, newrank, newsize, colors[oldrank], colors[newrank]);
	}
	
	// Завершение работы
	MPI_Finalize();
	return 0;
}