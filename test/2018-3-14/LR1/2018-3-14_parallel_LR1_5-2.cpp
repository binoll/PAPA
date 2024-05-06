/*
   5. Реализовать при помощи отправки сообщений типа "точка-точка" следующие схемы коммуникации процессов:
      1. Передача данных по кольцу, два варианта: “эстафетная палочка” (очередной процесс дожидается сообщения
         от предыдущего и потом посылает следующему) и “сдвиг”(одновременная посылка и прием сообщений).
      2. master-slave (все процессы общаются с одним).
      3. Пересылка данных от каждого процесса каждому.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define MAX_BUF_SIZE 4096

int main(int argc, char *argv[])
{
   int rank, n, opt, master_rank, count;
   char* sendbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
   char* recvbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
   MPI_Status status;

   while ((opt = getopt(argc, argv, "m:")) != -1) {
      switch (opt) {
      case 'm':
         master_rank = atoi(optarg);
         break;
      default:
         fprintf(stderr, "Invalid parameters. Getopt return value: 0%o\n", opt);
         exit(EXIT_FAILURE);
      }
   }
   if (argc != 3) {
         fprintf(stderr, "Invalid parameters. Set option -m <master_rank>\n");
         exit(EXIT_FAILURE);
   }

   MPI_Init(&argc, &argv);

   MPI_Comm_size(MPI_COMM_WORLD, &n);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   if (rank == master_rank) {
      sprintf(sendbuf, "Hello from MASTER %d", rank);

      int i;
      for (i = 1; i < n; i++) {
         MPI_Sendrecv(sendbuf, strlen(sendbuf), MPI_CHAR, (master_rank + i) % n,   1,
                      recvbuf, MAX_BUF_SIZE,    MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG,
                      MPI_COMM_WORLD, &status);

         MPI_Get_count(&status, MPI_CHAR, &count);

         printf("[%d] <- %.*s\n", rank, count, recvbuf);

      }
   }
   else {
      sprintf(sendbuf, "Hello from sleave %d", rank);

      MPI_Sendrecv(sendbuf, strlen(sendbuf), MPI_CHAR, master_rank, 1,
                   recvbuf, MAX_BUF_SIZE,    MPI_CHAR, master_rank, 1,
                   MPI_COMM_WORLD, &status);

      MPI_Get_count(&status, MPI_CHAR, &count);

      printf("[%d] <- %.*s\n", rank, count, recvbuf);
   }

   MPI_Finalize();

   exit(EXIT_SUCCESS);
}