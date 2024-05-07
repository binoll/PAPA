
/*
   6. С использованием  неблокирующих операций осуществить транспонирование
      квадратной матрицы, распределенной между процессорами по строкам.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
   int rank, n;
   int i, j, k;

   MPI_Init(&argc, &argv);

   MPI_Comm_size(MPI_COMM_WORLD, &n);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   MPI_Status status[2 * n];
   MPI_Request requests[2 * n];

   char* sendbuf = (char*)malloc(n * sizeof(char));
   char* recvbuf = (char*)malloc(n * sizeof(char));

   for(k = 0; k < n; k++) {
      sendbuf[k] = (char)(65 + k + n * rank);
   }

   printf("[%d] %.*s\n", rank, n, sendbuf);
   
   MPI_Barrier(MPI_COMM_WORLD);
   if (rank == 0) { printf("\n"); }

   for (i = 0; i < n; i++) {
       MPI_Isend(&sendbuf[i], 1, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests[i]);
       MPI_Irecv(&recvbuf[i], 1, MPI_CHAR, i, 0, MPI_COMM_WORLD, &requests[n + i]);
   }

   MPI_Waitall(2 * n, requests, status);

   printf("[%d] %.*s\n", rank, n, recvbuf);

   MPI_Finalize();

   exit(EXIT_SUCCESS);
}