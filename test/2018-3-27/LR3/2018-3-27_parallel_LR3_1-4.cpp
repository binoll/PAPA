#include <iostream>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"


int main(int argc, char** argv) 
{
  int rank,size, i;      

  // Параметр для получения аргумента
  int opt;
  int m_size = 0;
  while ((opt = getopt(argc, argv, "s:")) != -1) 
  {
    switch (opt) 
    {
      // Параметр s отвечает за длинну массива, 
      // расположенном в каждом процессе
      case 's':
        m_size = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Unrecognized option!\n");
        break;
    }
  }

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Проверим, считался ли аргумент
  if (m_size == 0)
    m_size = size;

  // Подготовка локальных данных
  int send[m_size];
  for (i = 0; i < m_size; i++)
    send[i] = rank + 1;
  
  int recv[m_size]; 
  //send = rank + 1;
  for (i = 0; i< m_size; i++)
    printf("Process %d has %d local data %d\n", rank, i, send[i]);

  // Вызываем функцию MPI_Scan
  //MPI_Scan(sendbuf, recvbuf, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  MPI_Scan(&send, &recv, m_size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  //Ожидание передачи
  MPI_Barrier(MPI_COMM_WORLD);

  for (i = 0; i < m_size; i++)
    printf("Process %d: has received %d data: %d \n", rank, i,  recv[i]);

  //Ожидание вывода
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return EXIT_SUCCESS;
}