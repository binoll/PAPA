/*
   3. Смоделировать последовательный двусторонний обмен сообщениями
      между двумя процессами, замерить с помощью функции
      MPI_Wtime() время на одну итерацию обмена, определить
      зависимость времени от длины сообщения.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define MAX_BUF_SIZE 1073741824  // 1GB
#define MAX_RPINT 100
#define ROOT_RANK 0
#define N 2

int main(int argc, char *argv[])
{
   int rank, n, opt, count, i, size = MAX_BUF_SIZE;
   char** messages = (char**)malloc(N * sizeof(char*));
   char* sendbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
   char* recvbuf = (char*)malloc(MAX_BUF_SIZE * sizeof(char));
   int tag0 = 0, tag1 = 1;
   double start, stop, time, times[N];
   MPI_Status status;

   while ((opt = getopt(argc, argv, "s:r:g:")) != -1) {
      switch(opt) {
         case 's':
            messages[0] = (char*)malloc(strlen(optarg));
            sprintf(messages[0], optarg);
            break;
         case 'r':
            messages[1] = (char*)malloc(strlen(optarg));
            sprintf(messages[1], optarg);
            break;
         case 'g':
             size = atoi(optarg);

             messages[0] = (char*)malloc(size * sizeof(char));
             messages[1] = (char*)malloc(size * sizeof(char));

             for (i = 0; i < size; i++) {
                 messages[0][i] = (char)(65 + i % 63);
                 messages[1][i] = (char)(48 + i % 10);
             }
             break;
         default:
            fprintf(stderr, "Invalid parameters. Getopt return value: 0%o\n", opt);
            exit(EXIT_FAILURE);
      }
   }
   if (argc <= 1) {
      fprintf(stderr, "Invalid parameters. Set options -s <message1> -r <message2> or option -g <len_of_message>\n");
      exit(EXIT_FAILURE);
   }
   
   MPI_Init(&argc, &argv);

   MPI_Comm_size(MPI_COMM_WORLD, &n);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   if (n != N) {
      fprintf(stderr, "Invalid count of procces. Set options -np %d\n", N);
      exit(EXIT_FAILURE);
   }

   snprintf(sendbuf, size + 1, messages[rank]);

   size = strlen(sendbuf);

   printf("[%d] -> (%3d byte) %.*s\n", rank, size, size <= MAX_RPINT ? size : 0, messages[rank]);

   start = MPI_Wtime();
   if (rank == ROOT_RANK) {
      MPI_Send(sendbuf, size,         MPI_CHAR, 1, tag0, MPI_COMM_WORLD);
      MPI_Recv(recvbuf, MAX_BUF_SIZE, MPI_CHAR, 1, tag1, MPI_COMM_WORLD, &status);
   }
   else {
      MPI_Recv(recvbuf, MAX_BUF_SIZE, MPI_CHAR, 0, tag0, MPI_COMM_WORLD, &status);
      MPI_Send(sendbuf, size,         MPI_CHAR, 0, tag1, MPI_COMM_WORLD);
   }
   stop = MPI_Wtime();

   MPI_Get_count(&status, MPI_CHAR, &count);

   printf("[%d] <- (%3d byte) %.*s\n", rank, count, count <= MAX_RPINT ? count : 0, recvbuf);

   time = stop - start;

   MPI_Gather(&time, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, ROOT_RANK, MPI_COMM_WORLD);

   if (rank == ROOT_RANK) {
      sleep(1);
      printf("\nAvg time: %f sec\n", (times[0] + times[1]) / n);
   }

   MPI_Finalize();

   exit(EXIT_SUCCESS);
}