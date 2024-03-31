#include <stdio.h>
#include "mpi.h"

#define ARR_SIZE 5
#define BUF_SIZE 100
#define ROOT_RANK 0
int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);


    int size, rank, position;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int a[ARR_SIZE];
    char b[ARR_SIZE];
    char buf[BUF_SIZE];

    if(rank == 0)
    {
        position = 0;
        for(int i = 0; i < ARR_SIZE; i++)
        {
            a[i] = size * rank + i;
            b[i] = char(int('A') + size * rank + i);
        }

        MPI_Pack(a, ARR_SIZE, MPI_INT, buf, BUF_SIZE, &position, MPI_COMM_WORLD);
        MPI_Pack(b, ARR_SIZE, MPI_CHAR, buf, BUF_SIZE, &position, MPI_COMM_WORLD);

        printf("[ PACK ] rank=%d, char array: \"%s\", int array: [ ", rank, b);
        for(int i = 0; i < ARR_SIZE; i++)
            printf("%d ", i);
        printf("]\n");
    }

    MPI_Bcast(buf, BUF_SIZE, MPI_PACKED, ROOT_RANK, MPI_COMM_WORLD);

    if(rank != 0)
    {
        position = 0;
        MPI_Unpack(buf, BUF_SIZE, &position, a, ARR_SIZE, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(buf, BUF_SIZE, &position, b, ARR_SIZE, MPI_CHAR, MPI_COMM_WORLD);

        printf("[UNPACK] rank=%d, char array: \"%s\", int array: [ ", rank, b);
        for(int i = 0; i < ARR_SIZE; i++)
            printf("%d ", a[i]);
        printf(" ]\n");
    }
    MPI_Finalize();
}
