#include "mpi.h"
#include "stdio.h"
#include "cstdlib"

#define start_of_alphabet 97
#define size 10

int main(int argc, char *argv[])
{
	int oldrank, newrank, oldsize, newsize;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	
	char* letters = (char*)malloc(sizeof(char)*size);
	for (int i = 0; i < size; ++i)
		letters[i] = (char)(i+start_of_alphabet);
	int colors[size] = {0, MPI_UNDEFINED, 3, 0, 3, 0, 0, 5, 3, MPI_UNDEFINED};
	int keys[size] = {3, 1, 2, 5, 1, 1, 1, 2, 1, 0};
	
	MPI_Init(&argc, &argv);
	
	// Получаем ранг текущего процессора
	if (MPI_Comm_rank( MPI_COMM_WORLD, &oldrank)) perror("Err at Comm_rank :( \n");	
	if (MPI_Comm_size(MPI_COMM_WORLD, &oldsize)) perror("Err at Comm_size :( \n");
	
	MPI_Comm newcomm;

	if(MPI_Comm_split( MPI_COMM_WORLD,  colors[oldrank], keys[oldrank], &newcomm)) 
		perror("Err at split :( \n");
	
	if (newcomm != MPI_COMM_NULL)
	{
		if (MPI_Comm_rank(newcomm, &newrank)) perror("Err at Comm_rank :( \n");
		printf("My old rank was %d, my new rank is %d ---> ",oldrank, newrank);
		
		if (MPI_Comm_size(newcomm, &newsize)) perror("Err at Comm_size :( \n");
		printf("My old groupsize was %d, my new groupsize is %d\n",oldsize, newsize);
	}
	else printf("My old rank was %d, but now i dont have group((\n",oldrank);
	
	MPI_Finalize();
}