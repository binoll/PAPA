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
   int rank, n, rank_from, rank_to, count;
   char* sendbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
   char* recvbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
   MPI_Status status;

   MPI_Init(&argc, &argv);

   MPI_Comm_size(MPI_COMM_WORLD, &n);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   rank_from = (n + rank - 1) % n;
   rank_to = (rank + 1) % n;
   sprintf(sendbuf, "Hello from %d", rank);

   MPI_Sendrecv(sendbuf, strlen(sendbuf), MPI_CHAR, rank_to,   1,
                recvbuf, MAX_BUF_SIZE,    MPI_CHAR, rank_from, MPI_ANY_TAG,
                MPI_COMM_WORLD, &status);

   MPI_Get_count(&status, MPI_CHAR, &count);

   printf("(%d) -> [%d] -> (%d) msg: \"%.*s\"\n", rank_from, rank, rank_to, count, recvbuf);

   MPI_Finalize();

   exit(EXIT_SUCCESS);
}