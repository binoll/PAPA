/*
 *   3. Разработать параллельную программу, демонстрирующую передачу элементов разного типа
 *      из нескольких областей памяти с использованием функций MPI_Pack и MPI_Unpack.
 *      Продемонстрировать передачу упакованных элементов с использованием операций коллективного взаимодействия.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define MAX_BUF_SIZE 1000

int main(int argc, char** argv)
{
    int rank, n, position = 0;
    char buff[MAX_BUF_SIZE];

    // Data to be sent in a package
    double x, y;
    int a[2];
    char msg;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Packaging of data in the process with rank = 0
    if (!rank) {
        x = 1.1; y = 2.2;
        a[0] = 3; a[1] = 4;
        msg = 'A';
        MPI_Pack(&x, 1, MPI_DOUBLE, buff, MAX_BUF_SIZE, &position, MPI_COMM_WORLD);
        MPI_Pack(&y, 1, MPI_DOUBLE, buff, MAX_BUF_SIZE, &position, MPI_COMM_WORLD);
        MPI_Pack(a, 2, MPI_INT, buff, MAX_BUF_SIZE, &position, MPI_COMM_WORLD);
        MPI_Pack(&msg, 1, MPI_CHAR, buff, MAX_BUF_SIZE, &position, MPI_COMM_WORLD);
        printf("# Source data in process with rank = 0: "
               "\n Double: \t<%.2f> <%.2f>\n Massive int: \t[%d, %d]\n Char: \t\t<%c>\n\n", x, y, a[0], a[1], msg);
    }
    // To correctly display the result on the screen by printf()
    MPI_Barrier(MPI_COMM_WORLD);
    // Distribution of packaged data to all processes
    MPI_Bcast(buff, MAX_BUF_SIZE, MPI_PACKED, 0, MPI_COMM_WORLD);
    if (!rank)
        printf("# Received data after unpacking the package in each process\n");
    // Unpacking packed data from process 0
    position = 0;
    MPI_Unpack(buff, MAX_BUF_SIZE, &position, &x, 1, MPI_DOUBLE, MPI_COMM_WORLD);
    MPI_Unpack(buff, MAX_BUF_SIZE, &position, &y, 1, MPI_DOUBLE, MPI_COMM_WORLD);
    MPI_Unpack(buff, MAX_BUF_SIZE, &position, a, 2, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buff, MAX_BUF_SIZE, &position, &msg, 1, MPI_CHAR, MPI_COMM_WORLD);

    printf("[%d] <%.2f> <%.2f> [%d, %d] %c\n", rank, x, y, a[0], a[1], msg);
    MPI_Finalize();
    exit(EXIT_SUCCESS);
}