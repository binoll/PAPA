#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/types.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int i, j, k, opt, n, rank, size, tag = 0, rand_ = 10;
    
    // Параметр для получения аргумента
    while ((opt = getopt(argc, argv, "s:")) != -1)
    {
        switch (opt)
        {
        case 's':
            size = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Bad argument");
            exit(EXIT_FAILURE);
        }
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // Задание строки
    int a[size];
        
    // Заполнение строки
    srand(rand_);
    
    for (i=0; i < size; i++)
    {
        a[i] = rand() % 100;
    }

    // // Нулевой ранг выведет матрицу
    // if (rank == 0) 
    // {
    //   printf("Rank %d send matrix:\n", rank);
    //   for (i = 0; i < size; i++) 
    //     {
    //       for (j = 0; j < size; j++) 
    //       {
    //         printf(" %d", a[i][j]);
    //       }
    //       printf ("\n");
    //     }
    //   printf ("\n\n");
    // }  

    // Создадим производный тип данных для получения с помощью вектора
    MPI_Datatype ROW;
    MPI_Type_contiguous(size, MPI_INT, &ROW);
    MPI_Type_commit(&ROW);

    // Мы создали только тип строчек, 
    // передадим матрицу по строчкам всем процессам
    
    int b[size * size];
    
    MPI_Gather(a, 1, ROW, b, 1, ROW, 0, MPI_COMM_WORLD);

    if(rank == 0)
    {  
      for (int i = 0; i < size; ++i)
      {
          for (int j = 0; j < size; ++j) {
              printf("%d ", b[i + j]);
          }
          printf("\n");
      }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Type_free(&ROW);
    MPI_Finalize();
    return 0;
}