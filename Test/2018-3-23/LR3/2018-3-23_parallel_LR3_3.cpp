#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stddef.h>
#include "mpi.h"

int main(int argc, char **argv)
{
  int rank, packsize, position;
  int a;
  double b;
  bool c;

  char packbuf[20];
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  if (rank == 0) 
  {
    a = 20;
    b = 1.11;
    c = false;
    
    // position 	текущая позиция в буфере в байтах (целое)
    // MPI_PACK(inbuf, incount, datatype, outbuf, outsize, position, comm)
    packsize = 0; 
    
    // Добавление даты в буффер
    printf("Размер посылки до добавления: %d\n", packsize);
    MPI_Pack(&a, 1, MPI_INT, packbuf, 20, &packsize, MPI_COMM_WORLD);
    printf("Размер посылки после 1 добавления: %d\n", packsize);
    MPI_Pack(&b, 1, MPI_DOUBLE, packbuf, 20, &packsize, MPI_COMM_WORLD);
    printf("Размер посылки после 2 добавления: %d\n", packsize);
    MPI_Pack(&c, 1, MPI_INT, packbuf, 20, &packsize, MPI_COMM_WORLD);
    printf("Размер посылки после 3 добавления: %d\n", packsize);
  }

  MPI_Bcast(&packsize, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(packbuf, packsize, MPI_PACKED, 0, MPI_COMM_WORLD);

  // Вывод передачи
  if (rank == 1) 
  {
    printf("Rank %d got values:\n", rank);
    position = 0; /* Unpack data */
    MPI_Unpack(packbuf, packsize, &position, &a, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(packbuf, packsize, &position, &b, 1, MPI_DOUBLE, MPI_COMM_WORLD);
    MPI_Unpack(packbuf, packsize, &position, &c, 1, MPI_INT, MPI_COMM_WORLD);
    printf("%d, %f, %s", a, b, c ? "true" : "false");
    printf("\n");
  }
  
  MPI_Finalize();
  return 0;
}
