/*
Разработать параллельную программу, 
демонстрирующую передачу элементов 
разного типа из нескольких областей памяти 
с использованием функций MPI_Pack и MPI_Unpack. 
Продемонстрировать передачу упакованных элементов 
с использованием операций коллективного взаимодействия.
*/

#include <stdio.h>   // printf()
#include <mpi.h>     // MPI functions
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{

	MPI_Init(&argc, &argv);
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	char out_buf[20];
	
	if (rank == 0)
	{
		int first = 123;
		char second = 'a';
		int size = sizeof(int) + sizeof(char);
		int position = 0;
		MPI_Pack(&first, 1, MPI_INT, out_buf, 20, &position, MPI_COMM_WORLD);
		MPI_Pack(&second, 1, MPI_CHAR, out_buf, 20, &position, MPI_COMM_WORLD);
	}
	MPI_Bcast(out_buf, 20, MPI_PACKED, 0, MPI_COMM_WORLD);

	if (rank != 0)
	{
		int position = 0;
		int first;
		char second;
		MPI_Unpack(out_buf, 20, &position, &first, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(out_buf, 20, &position, &second, 1, MPI_CHAR, MPI_COMM_WORLD);
		printf("Unpack in rank %d. First: %d, second: %c\n", rank, first, second);
	}
	MPI_Finalize();
	return 0;
}