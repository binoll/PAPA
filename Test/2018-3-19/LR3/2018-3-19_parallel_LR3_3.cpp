/*
Разработать параллельную программу, демонстрирующую передачу
элементов разного типа из нескольких областей памяти с использованием
функций MPI_Pack и MPI_Unpack.Продемонстрировать передачу упакованных
элементов с использованием операций коллективного взаимодействия
*/
#include <stdio.h>
#include "mpi.h"

int main(int argc, char** argv)
{
    int n, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int a;
    float b;
    char c;
    double d;

    int size = 1000;
    //начало выходного буфера
    char packbuf[1000];

    //текущая позиция в буфере в байтах
    int position;

    if (rank == 0)
    {
        a = 22;
        b = 1.8;
        c = 'D';
        d = 2.2;
        position = 0;

        printf("size pack before added: %d\n", position);
        MPI_Pack(&a, 1, MPI_INT, packbuf, size, &position, MPI_COMM_WORLD);
        printf("size pack afater int added: %d\n", position);
        MPI_Pack(&b, 1, MPI_FLOAT, packbuf, size, &position, MPI_COMM_WORLD);
        printf("size pack afater float added: %d\n", position);
        MPI_Pack(&c, 1, MPI_CHAR, packbuf, size, &position, MPI_COMM_WORLD);
        printf("size pack afater char added: %d\n", position);
        MPI_Pack(&d, 1, MPI_DOUBLE, packbuf, size, &position, MPI_COMM_WORLD);
        printf("size pack afater double added: %d\n", position);
    }

    MPI_Bcast(packbuf, size, MPI_PACKED, 0, MPI_COMM_WORLD);

    if (rank != 0)
    {
        position = 0;
        MPI_Unpack(packbuf, size, &position, &a, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(packbuf, size, &position, &b, 1, MPI_FLOAT, MPI_COMM_WORLD);
        MPI_Unpack(packbuf, size, &position, &c, 1, MPI_CHAR, MPI_COMM_WORLD);
        MPI_Unpack(packbuf, size, &position, &d, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        printf("rank: %d recv:\nint: %d\tfloat: %f\tchar: %c\tdouble: %f\n", rank, a, b, c, d);
    }


    MPI_Finalize();
    return 0;
}