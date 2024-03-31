#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[])
{
   int n, rank, namelen;
   char processor_name[MPI_MAX_PROCESSOR_NAME];
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &n);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Get_processor_name(processor_name, &namelen);
   MPI_Finalize();
   if (rank == 0) {
	   printf("node name - %s, rank - %d, size - %d \n", processor_name, rank, n);
   }
   else {
	   printf("node name - %s, rank - %d, size - %d \n", processor_name, rank, n);   
   }
   return 0;
}	

