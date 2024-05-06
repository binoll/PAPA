
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdlib.h>  // чтобы пользоваться функцией rand
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>

#include "mpi.h"

using namespace std;

int main(int argc, char **argv)
{
  int rank, packsize, position;
  int a;
  double b;
  bool c;

  // Задание типы uint8_t для хранения данных
  int packbuf[100];
  
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
    using namespace std;
    // Добавление даты в буффер
    //printf("Размер посылки до добавления: %d \n", packsize);
    MPI_Pack(&a, 1, MPI_INT, packbuf, 100, &packsize, MPI_COMM_WORLD);
    //printf("Размер посылки после 1 добавления: %d \n", packsize);
    MPI_Pack(&b, 1, MPI_DOUBLE, packbuf, 100, &packsize, MPI_COMM_WORLD);
    //printf("Размер посылки после 2 добавления: %d \n", packsize);
    MPI_Pack(&c, 1, MPI_INT, packbuf, 100, &packsize, MPI_COMM_WORLD);
    //printf("Размер посылки после 3 добавления: %d \n", packsize);
  }

  // Передача от 0 процесса всем остальным
  // Сначала перешлем размер посылки, следом данные
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
    printf("%d, %f, %s", a, b, c?"true":"false");
    printf("\n");
  }
  
  MPI_Finalize();
  return 0;
}