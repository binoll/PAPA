/*
 *   1. Разработать параллельную программу, использующую базовые конструкции MPI
 *      (получение числа процессоров, своего ранга и имени вычислительного модуля).
 *      Программа должна производить вывод информации в стандартный поток вывода,
 *      а сообщения об ошибках – в стандартный поток ошибок.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

int main( int argc, char *argv[])
{
    int n, rank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &n);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(processor_name, &namelen);

	printf("[All process - %d] Current rank - %d, node name - %s\n",n, rank, processor_name);

    MPI_Finalize();

    exit(EXIT_SUCCESS);
}
