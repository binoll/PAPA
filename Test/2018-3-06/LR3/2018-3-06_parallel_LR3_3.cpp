//#define _CRT_SECURE_NO_WARNINGS
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "mpi.h"
#include <time.h>

int main(int argc, char* argv[]) {
    int rank, nproc, b_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int a = {rand() * 3};
    float a_1 = { (float(rand()) + 10)/3};
    char b = 'new';
    int position = 0;
    b_size = 50;
    char buffer[b_size];
    if (rank == 0) {
        MPI_Pack(
            &a, // входной буфер
            1, // число входных компонент
            MPI_INT, // тип входных компонент
            buffer, // выходной буфер
            b_size, // размер выходного буфера в байтах
            &position, // текущая позиция в буфере, в байтах
            MPI_COMM_WORLD);
        MPI_Pack(&b, 1, MPI_CHAR, buffer, b_size, &position, MPI_COMM_WORLD);
        MPI_Pack(&a_1, 1, MPI_FLOAT, buffer, b_size, &position, MPI_COMM_WORLD);
    }
    MPI_Bcast(buffer, b_size, MPI_PACKED, 0, MPI_COMM_WORLD);
    int a_new;
    float a_1_new;
    char b_new;
    position = 0;
    MPI_Unpack(
        buffer, // входной буфер
        b_size, // размер входного буфера в байтах
        &position, // текущая позиция в буфере, в байтах
        &a_new, // выходной буфер
        1, // число выходных компонент
        MPI_INT, // тип выходных компонент
        MPI_COMM_WORLD);
    MPI_Unpack(buffer, b_size, &position, &b_new, 1, MPI_CHAR, MPI_COMM_WORLD);
    MPI_Unpack(buffer, b_size, &position, &a_1_new, 1, MPI_FLOAT, MPI_COMM_WORLD);

    printf("\nrank=%d : %d --> %c --> %f\n", rank, a_new, b_new, a_1_new);
    MPI_Finalize();
    exit(EXIT_SUCCESS);
}
