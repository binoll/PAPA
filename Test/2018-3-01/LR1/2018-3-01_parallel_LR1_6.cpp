#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>
#define MAXPROC 64

int main(int argc, char** argv)
{
    srand(time(NULL));
    int rank, size, nl, i, j, ii, jj, ir, irr;
    int c;
    int N = (rand() + 1) % 10; // Генерация случайного размера квадратной матрицы
    MPI_Status status;
    MPI_Request req[MAXPROC * 2];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Определяем размер коммуникатора
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Определяем ранг процесса
    nl = N;
    int* a = (int*)calloc(size * nl * nl, sizeof(int)); // Выделяем блок памяти размером (size * nl * nl) * sizeof(int) и заполняем нулями
    int* b = (int*)calloc(size * nl * nl, sizeof(int));

    // Заполняем область памяти случайными значениями - Инициализируем матрицу
    for (i = 0; i < nl; i++) {
        ii = i + rank * nl;
        if (ii < N)
            for (j = 0; j < N; j++)
                *(a + j * nl + i) = rand() % 10;
    }

    // Распределяем строки между процессами
    for (ir = 0; ir < size; ir++) {
        MPI_Irecv(b + ir * nl * nl, nl * nl, MPI_INT, ir, 1, MPI_COMM_WORLD, &req[ir]);
        MPI_Isend(a + ir * nl * nl, nl * nl, MPI_INT, ir, 1, MPI_COMM_WORLD, &req[ir + size]);
    }

    // Осуществляем транспонирование матрицы
    for (irr = 0; irr < size; irr++) {
        MPI_Waitany(size, req, &ir, &status);
        ir = status.MPI_SOURCE;
        for (i = 0; i < nl; i++) {
            for (j = i + 1; j < nl; j++) {
                c = *(b + i * nl + j + ir * nl * nl);
                *(b + i * nl + j + ir * nl * nl) = *(b + j * nl + i + ir * nl * nl);
                *(b + j * nl + i + ir * nl * nl) = c;
            }
        }
    }

    // Для единичного вывода
    if (rank == 0)
    {
        // Вывод исходной матрицы
        for (i = 0; i < nl; i++) {
            ii = i + rank * nl;
            if (ii < N)
                for (j = 0; j < N; j++)
                    printf("%i ", *(a + j * nl + i));
            printf("\n");
        }
        printf("\n");

        // Вывод транспонированной матрицы
        for (i = 0; i < nl; i++) {
            ii = i + rank * nl;
            if (ii < N)
                for (j = 0; j < N; j++)
                    printf("%i ", *(b + j * nl + i));
            printf("\n");
        }
    }
    free(a);  // Освобождение памяти
    free(b); // Освобождение памяти
    MPI_Finalize();
}