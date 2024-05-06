/*
1. Разработать параллельную программу, использующую базовые конструкции MPI 
(получение числа процессоров, своего ранга и имени вычислительного модуля).
Программа должна производить вывод информации в стандартный поток вывода, а 
сообщения об ошибках – в стандартный поток ошибок.
*/
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);

	int n, rank, namelen, func_res;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	if (MPI_Comm_size(MPI_COMM_WORLD, &n))
		perror("Error at MPI_Comm_size!\n");
	if (MPI_Comm_rank(MPI_COMM_WORLD, &rank))
		perror("Error at MPI_COMM_WORLD!\n");
	if (MPI_Get_processor_name(processor_name, &namelen))
		perror("Error at MPI_Get_processor_name!\n");

	if (rank == 0) {
		printf("Root proc:\tsize=%d, rank=%d, name=%s\n", n, rank, processor_name);
	}
	else {
		printf("Default proc:\tsize=%d, rank=%d, name=%s\n", n, rank, processor_name);
	}

	MPI_Finalize();
}
