/*
2. Разработать параллельную программу, осуществляющую разделение коммуникатора 
из лекционного примера. Программа должна вывести ранг в старом коммуникаторе, 
число процессоров и ранг в новом коммуникаторе. Если процесс не попал в новый 
коммуникатор, должен сообщить об этом.
*/
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int n1, rank1, namelen, func_res, key;
	if (MPI_Comm_size(MPI_COMM_WORLD, &n1))
		perror("Error at MPI_Comm_size!\n");
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank1))
		perror("Error at MPI_Comm_rank!\n");

	int color[] = {0,1,0,1,5,0,1,MPI_UNDEFINED,1,MPI_UNDEFINED};
	
	MPI_Comm new_comm;
	MPI_Comm_split(MPI_COMM_WORLD, color[rank1], rank1, &new_comm);
	if (new_comm != MPI_COMM_NULL)
	{
		int n2, rank2;
		if (MPI_Comm_size(new_comm, &n2))
			perror("Error at MPI_Comm_size!\n");
		if (MPI_Comm_rank(new_comm, &rank2))
			perror("Error at MPI_Comm_rank!\n");

		printf("old rank: %d, old count: %d\t--->\tnew rank:%d, new count:%d\n", rank1, n1, rank2, n2);
	}
	else
	{
		printf("old rank: %d, old count: %d\t--->\tPROCCESS WAS DENIED\n", rank1, n1);
	}

	MPI_Finalize();
	return 0;
}

