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

int main( int argc, char **argv )
{
    int      *sendbuf, *recvcounts;
    int      block_size;
    int      *recvbuf;
    int      size, rank, i;
    
    // Параметр для получения аргумента
    int opt;
    // Сколько элементов создает каждый процесс
    int m_size = 0;
    // Сколько столбцов берет каждый процесс
    int columns = 0;

    while ((opt = getopt(argc, argv, "s:c:")) != -1) 
    {
      switch (opt) 
      {
        // Параметр s отвечает за длинну массива, 
        // расположенном в каждом процессе
        case 's':
          m_size = atoi(optarg);
          break;
        case 'c':
          columns = atoi(optarg); 
          break;
        default:
          fprintf(stderr, "Unrecognized option!\n");
          break;
      }
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // Проверим, считался ли аргумент
    if (m_size == 0)
      m_size = size;
    if (columns == 0)
        columns = 1;

    if (columns * size != m_size)
    {
        fprintf(stderr, "Columns must divided correctly between proccesess!!! \n");
        return EXIT_FAILURE;
    }
    // Объявление данных для передачи
    // Тут мы задаем, сколько столбцов брать каждому процессу
    int count[size];
    for (i = 0; i < size; i++)
      count[i] = columns;

    // Заполняем значениями
    int values[m_size];
    for (i = 0; i < m_size; i++)
      values[i] = i;

    int result[columns];

    // Вывод первичных данных
    for (i = 0; i < m_size; i++)
      printf("%d ", values[i]);
    printf("\n");
    MPI_Barrier(MPI_COMM_WORLD);

    // Объявление MPI_Reduce_scatter
    MPI_Reduce_scatter(values, &result, count, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    
    // Вывод результата
    for (i = 0; i < columns; i++)  
        printf("Proc %d reduce %d: %d \n", rank, i, result[i]);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return EXIT_SUCCESS;
}