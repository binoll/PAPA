#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdlib.h>  // ����� ������������ �������� rand
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>


#include "mpi.h"


int main(int argc, char** argv) {
  
  int size, rank, namelen, i;
  
  // �������� ��� ��������� ���������
  int opt;
  int m_size = 0;
  while ((opt = getopt(argc, argv, "s:")) != -1) 
  {
    switch (opt) 
    {
      // �������� s �������� �� ������ �������, 
      // ������������� � ������ ��������
      case 's':
        m_size = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Unrecognized option!\n");
        break;
    }
  }

  // ������������� ���������, �����������
  MPI_Init(&argc,&argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // ��������, �������� �� ��������
  if (m_size == 0)
    m_size = size;

  // ��������� �����
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
  MPI_Barrier(MPI_COMM_WORLD);

  // Reduce all of the local sums into the global sum
  int recv[m_size];
  MPI_Reduce(&send, &recv, m_size, MPI_INT, MPI_SUM, 0,
             MPI_COMM_WORLD);

  // ����� ���������� �����
  if (rank == 0) 
  {
    printf("\n");
    printf("Result sum: \n");
    for (i = 0; i < m_size; i++) 
    {
      printf("%d ", recv[i]);
    }
    printf("\n");
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}
