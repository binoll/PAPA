#include <stdio.h>
#include "mpi.h"


int main(int argc, char * argv[]) 
{
	MPI_Init(&argc, &argv);
	int size, rank;
	int tag = 0;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Status status;
	MPI_Request request;

	int send_matrix[size][size];
	int recv_matrix[size][size];
	int recvbuf[size];
	int i, j;

	for (i; i < size; i++)
	{
		j = 0;
		for (j; j < size; j++)
		{
			send_matrix[i][j] = i; 
		}
	}
	// Тут мы задаем последовательный вывод матрицы для каждого процесса
	// Передаем что угодно, "флаг"
	int get = 1;
	if (rank == 0) 
	{
		printf("Исходная матрица\n");
		MPI_Send(&get, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		for (i = 0; i < size; i++) printf("%d ", send_matrix[rank][i]);
	} 
	else
	{
		int buffer;
		MPI_Status status;
		MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &buffer);
		if (buffer == 1) 
		{
			for (i = 0; i < size; i++) printf("%d ", send_matrix[rank][i]);

			MPI_Recv(&get, buffer, MPI_INT, MPI_ANY_SOURCE, 0, 
			MPI_COMM_WORLD, &status);

			if (rank + 1 != size) 
			{
				MPI_Send(&get, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
			}
		}
	}
	printf("\n");
	
	// Ждем вывода строчек
	MPI_Barrier(MPI_COMM_WORLD);
	i = 0;
	for (i; i < size; i++)
	MPI_Isend(&send_matrix[rank], size, MPI_INT, i, tag, MPI_COMM_WORLD, &request);

	// Ждем отправки
	MPI_Barrier(MPI_COMM_WORLD);
	i = 0;
	for (i; i < size; i++) 
	{
		MPI_Irecv(&recvbuf, size, MPI_INT, i, tag, MPI_COMM_WORLD, &request);
		j = 0;

		for (j; j < size; j++) recv_matrix[j][i] = recvbuf[j];
	}

	// Ждем получение 
	MPI_Barrier(MPI_COMM_WORLD);
	// Вывод матрицы
	if (rank == 0) 
	{
		printf("Результирующая матрица\n");
		i = 0;
		for (i; i < size; i++) 
		{
			j = 0;
			for (j; j < size; j++)
			printf("%d ", recv_matrix[i][j]);
			printf("\n");
		}
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
}