#include <stdio.h>
#include <unistd.h>
#include "mpi.h"


int main(int argc, char* argv [])
{
	int rank, size;
 
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );

	int send = rank;
	printf("Rank %d ready\n", rank);
	int recv[size];
	int i;

	MPI_Request request_s[size];
	MPI_Request request_g[size];
	MPI_Status status[size];

	for (i=0; i < size; i++)
	{
		MPI_Isend(&send, 10, MPI_INT, i, 0, MPI_COMM_WORLD, &request_s[i]); 
	}

	// Ожидаем
	MPI_Barrier(MPI_COMM_WORLD);
	for (i=0; i < size; i++)
	{ 
		MPI_Irecv(&recv[i], 10, MPI_INT, i, 0, MPI_COMM_WORLD, &request_g[i]);
	}
	MPI_Waitall(size, request_g, status);
	// Тут мы задаем последовательный вывод матрицы для каждого процесса
	// Передаем что угодно, "флаг"
	int get = 1;
	if (rank == 0) 
	{
		printf("Rank %d got:\n", rank);
		MPI_Send(&get, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		for (i = 0; i < size; i++) printf("%d ", recv[i]);
	} 
	else
	{
		int buffer;
		MPI_Status status;
		MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &buffer);

		if (buffer == 1) 
		{
			printf("Rank %d got:\n", rank);
			for (i = 0; i < size; i++) printf("%d ", recv[i]);

			MPI_Recv(&get, buffer, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

			if (rank + 1 != size) 
			{
				MPI_Send(&get, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
			}
		}
	}
	printf("\n");

	// Ждем вывода строчек
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
