/*
   1. Разработать параллельную программу, использующую базовые конструкции MPI
      (получение числа процессоров, своего ранга и имени вычислительного модуля).
      Программа должна производить вывод информации в стандартный поток вывода,
      а сообщения об ошибках – в стандартный поток ошибок.
*/

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
   int n, rank, namelen, status;
   char processor_name[MPI_MAX_PROCESSOR_NAME];

   status = MPI_Init(&argc, &argv);

   status = MPI_Comm_size(MPI_COMM_WORLD, &n);
   status = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   status = MPI_Get_processor_name(processor_name, &namelen);

   fprintf(stdout, "Total proccess: %d, rank = %d (name = %s)\n", n, rank, processor_name);
   
   status = MPI_Finalize();

   if (status != 0) {
      fprintf(stderr, "Error in MPI\n");
      exit(EXIT_FAILURE);
   }

   exit(EXIT_SUCCESS);
}