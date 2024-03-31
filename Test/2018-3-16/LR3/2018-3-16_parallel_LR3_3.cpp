#include <stdio.h>
#include "mpi.h"
#define BUFFER_SIZE 1024

int main(int argc, char** argv)
{
    int n, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int a, recva;
    float b, recvb;
    char c, recvc;
    double d, recvd;

    //начало выходного буфера
    char packbuf[BUFFER_SIZE];

    //текущая позиция в буфере в байтах
    int position, recvposition;

    if (rank == 0)
    {
        a = 155;
        b = 1.39;
        c = 'I';
        d = 2.788;
        position = 0;

        printf("size pack before added: %d\n", position);
        MPI_Pack(&a, 1, MPI_INT, packbuf, BUFFER_SIZE, &position, MPI_COMM_WORLD);
        printf("size pack afater int added: %d\n", position);
        MPI_Pack(&b, 1, MPI_FLOAT, packbuf, BUFFER_SIZE, &position, MPI_COMM_WORLD);
        printf("size pack afater float added: %d\n", position);
        MPI_Pack(&c, 1, MPI_CHAR, packbuf, BUFFER_SIZE, &position, MPI_COMM_WORLD);
        printf("size pack afater char added: %d\n", position);
        MPI_Pack(&d, 1, MPI_DOUBLE, packbuf, BUFFER_SIZE, &position, MPI_COMM_WORLD);
        printf("size pack afater double added: %d\n", position);
    }
    MPI_Bcast(packbuf, BUFFER_SIZE, MPI_PACKED, 0, MPI_COMM_WORLD);
    if (rank != 0)
    {
        recvposition = 0;
        MPI_Unpack(packbuf, BUFFER_SIZE, &recvposition, &recva, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(packbuf, BUFFER_SIZE, &recvposition, &recvb, 1, MPI_FLOAT, MPI_COMM_WORLD);
        MPI_Unpack(packbuf, BUFFER_SIZE, &recvposition, &recvc, 1, MPI_CHAR, MPI_COMM_WORLD);
        MPI_Unpack(packbuf, BUFFER_SIZE, &recvposition, &recvd, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        printf("rank: %d recv:\nint: %d\tfloat: %f\tchar: %c\tdouble: %f\n", rank, recva, recvb, recvc, recvd);
    }


    MPI_Finalize();
    return 0;
}
