#include <stdio.h>
#include "mpi.h"

int main(int argc, char** argv) 
{
	int size, rank, namelen;
	// Минимальная длина названия процесса
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	// Инициализация программы, коммутатора

	MPI_Init(&argc,&argv);
	// MPI_COMM_WORLD - глобальная коммуникационная среда
	// size - получение общего числа, rank - процесса

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

	fprintf(stderr, "My rank: %d, my proc: %s\n", rank, processor_name);
	printf("My rank: %d, my proc: %s. Count: %d\n", rank, processor_name, size);

	// Завершение программы
	MPI_Finalize();
	return 0;
}
