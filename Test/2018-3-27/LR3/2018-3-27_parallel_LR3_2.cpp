
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

int main(int argc, char *argv[])
{
    int i, j, k, rank, size, tag = 0, rand_ = 10;
    
    // Параметр для получения аргумента
    int opt;
    int m_size;
    while ((opt = getopt(argc, argv, "s:r:")) != -1) 
    {
      switch (opt) 
      {

        case 's':
          m_size = atoi(optarg);
          break;
        case 'r':
          rand_ = atoi(optarg);
        default:
          fprintf(stderr, "Unrecognized option!\n");
          break;
      }
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Зададим матрицы для перемножения
    // double *send = malloc(m_size * m_size * sizeof(double));

    
    // Задание матрицы
    int **a = new int*[m_size];
    for (int i = 0; i < m_size; i++)
      a[i] = new int[m_size];
      
    /*int **a = (int **)malloc(m_size*sizeof(int *));
    for(int i = 0; i < m_size; i++) 
    {
      a[i] = (int *)malloc(m_size*sizeof(int));
    }
    */

    // Заполнение матрицы
    srand(rand_);
    if (rank == 0)
    {
      for (i=0; i < m_size; i++)
      {
          for (j=0; j < m_size; j++)
          {
              a[i][j] = rand() % 100;
          }
      }
    }


    // Нулевой ранг выведет матрицу
    if (rank == 0) 
    {
      printf("Rank %d send matrix:\n", rank);
      for (i = 0; i < m_size; i++) 
        {
          for (j = 0; j < m_size; j++) 
          {
            printf(" %d", a[i][j]);
          }
          printf ("\n");
        }
      printf ("\n\n");
    }  

    // Создадим производный тип данных для получения с помощью вектора
    MPI_Datatype ROW;
    MPI_Type_contiguous(m_size, MPI_INT, &ROW);
    MPI_Type_commit(&ROW);

    // Мы создали только тип строчек, 
    // передадим матрицу по строчкам всем процессам
    for (i=0; i < m_size; i++)
      MPI_Bcast(&a[i][0], 1, ROW, 0, MPI_COMM_WORLD);

    // Первый ранг выведет матрицу
    if (rank == 1) 
    {
      printf("Rank %d get matrix:\n", rank);
      for (i = 0; i < m_size; i++) 
        {
          for (j = 0; j < m_size; j++) 
          {
            printf(" %d", a[i][j]);
          }
          printf ("\n");
        }
      printf ("\n\n");
    }    

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Type_free(&ROW);
    MPI_Finalize();
    return 0;
}