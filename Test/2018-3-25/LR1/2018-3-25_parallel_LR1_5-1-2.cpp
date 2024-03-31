/*
Реализовать при помощи отправки сообщений типа "точка-точка" 
следующие схемы коммуникации процессов: “сдвиг”(одновременная 
посылка и прием сообщений) 
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

  
  int tag = 0;
  MPI_Request request[2];
  MPI_Status status[2];
  int buf;
  
  if(rank == size - 1){
    MPI_Isend(&rank, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(&buf, 1, MPI_INT, rank - 1, tag, MPI_COMM_WORLD, &request[1]);
    MPI_Wait(&request[1], &status[1]);
    printf("%d Send: %d, recv: %d, buf: %d\n",rank, 0, rank - 1, buf);
  }
  else if(rank == 0){
    MPI_Isend(&rank, 1, MPI_INT, rank + 1, tag, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(&buf, 1, MPI_INT, size - 1, tag, MPI_COMM_WORLD, &request[1]);
    MPI_Wait(&request[1], &status[1]);
    printf("%d Send: %d, recv: %d, buf: %d\n", rank, rank + 1, size - 1, buf);
  }
  else{
    MPI_Isend(&rank, 1, MPI_INT, rank + 1, tag, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(&buf, 1, MPI_INT, rank - 1, tag, MPI_COMM_WORLD, &request[1]);
    MPI_Wait(&request[1], &status[1]);
    printf("%d Send: %d, recv: %d, buf: %d\n", rank, rank + 1, rank - 1, buf);
  }

  MPI_Finalize();
  return 0;
}