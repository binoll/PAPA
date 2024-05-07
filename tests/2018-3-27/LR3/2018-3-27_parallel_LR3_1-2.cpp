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

int main(int argc, char** argv) {
  
  int size, rank, namelen, i;
  
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

  // Инициализация программы, коммутатора
  MPI_Init(&argc,&argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Проверим, считался ли аргумент
  if (m_size == 0)
    m_size = size;

  // Локальные числа
  int send[m_size];

  for (i = 0; i < m_size; i++) 
  {
    send[i] = rank + i;
  }

  for (i = 0; i < m_size; i++) 
  {
    printf("%d ", send[i]);
  }
  printf("\n");

  // Reduce all of the local sums into the global sum
  int recv[m_size];
  MPI_Allreduce(&send, &recv, m_size, MPI_INT, MPI_SUM,
                MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);
  // Вывод результата 1 процессом
  if (rank == 1) 
  {
    printf("Result sum: \n");
    for (i = 0; i < m_size; i++) 
    {
      printf("%d ", recv[i]);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return EXIT_SUCCESS;
}