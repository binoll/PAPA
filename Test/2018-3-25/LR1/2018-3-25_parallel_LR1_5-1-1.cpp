/*
Реализовать при помощи отправки сообщений типа "точка-точка" следующие схемы 
коммуникации процессов:
Передача данных по кольцу, два варианта: “эстафетная палочка” (очередной процесс
 дожидается сообщения от предыдущего и потом посылает следующему).
 */

#include <stdio.h>   // printf()
#include <mpi.h>     // MPI functions
#include <unistd.h>
#include <stdlib.h>
#define SIZE_ARRAY 1

int main(int argc, char** argv) {
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  int send_buf[SIZE_ARRAY] = {1};
  int recv_buf[SIZE_ARRAY];
  int tag = 0;
  MPI_Status status;
  if(rank == 0) {
    MPI_Send(send_buf, SIZE_ARRAY, MPI_INT, 1, tag, MPI_COMM_WORLD);
    MPI_Recv(recv_buf, SIZE_ARRAY, MPI_INT, size - 1, tag, MPI_COMM_WORLD, &status);
    printf("%d: send to 1, recv from %d\n", rank, size - 1);
  }
  else if (rank == size - 1) {
    MPI_Recv(recv_buf, SIZE_ARRAY, MPI_INT, rank - 1, tag, MPI_COMM_WORLD, &status);
    MPI_Send(send_buf, SIZE_ARRAY, MPI_INT, 0, tag, MPI_COMM_WORLD);
    printf("%d: recv from %d, send to 0\n", rank, rank - 1);
  }
  else {
    MPI_Recv(recv_buf, SIZE_ARRAY, MPI_INT, rank - 1, tag, MPI_COMM_WORLD, &status);
    MPI_Send(send_buf, SIZE_ARRAY, MPI_INT, rank + 1, tag, MPI_COMM_WORLD);
    printf("%d: recv from %d, send to %d\n", rank, rank - 1, rank + 1);
  }

  MPI_Finalize();
  return 0;
}