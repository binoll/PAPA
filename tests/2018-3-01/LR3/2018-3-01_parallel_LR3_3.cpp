#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

int main(int argc, char** argv)
{
    srand(time(NULL));
    int size, rank, position;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Размер коммуникатора
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Ранг каждого процесса
    int a = (rand() + rank) % 10; // 1. Переменная int рандомом для пересылки
    // 2. Переменная char для пересылки
    char b;
    if (rank == 0)
    {
        b = 'A';
    }
    else
    {
        b = 'B';
    }
    char buf[100]; // Буфер для упаковки данных
    printf("BEFORE:Process %d: a=%i, b=%c\n", rank, a, b); // Выводим что было ДО
    position = 0; // Текущая позиция в буфере
    if (rank == 0) {
        MPI_Pack(&a, 1, MPI_INT, buf, 100, &position, MPI_COMM_WORLD); // Упаковка первой переменной
        MPI_Pack(&b, 1, MPI_CHAR, buf, 100, &position, MPI_COMM_WORLD); // Упаковка второй переменной
    }
    MPI_Bcast(buf, 100, MPI_PACKED, 0, MPI_COMM_WORLD); // Передача от одного (root) ко всем
    position = 0; // Устанавливаем позицию в 0
    MPI_Unpack(buf, 100, &position, &a, 1, MPI_INT, MPI_COMM_WORLD); // Распаковка первой переменной
    MPI_Unpack(buf, 100, &position, &b, 1, MPI_CHAR, MPI_COMM_WORLD); // Распаковка второй переменной
    printf("AFTER:Process %d: a=%i, b=%c\n", rank, a, b); // Выводим что стало после
    MPI_Finalize();
}