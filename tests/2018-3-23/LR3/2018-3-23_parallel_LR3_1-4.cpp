#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int opt, size, n, rank;
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

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n);

    int sendbuf[size];
    for (int i = 0; i < size; ++i)
    {
      sendbuf[i] = rank + i;
    }

    printf("rank %d, send: ", rank);
    for(int i = 0; i < size; ++i)
    {
      printf("%d ", sendbuf[i]);
    }
    printf("\n");
    
    int recvbuf[size];
 
    MPI_Scan(sendbuf, recvbuf, size, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    printf("rank:%d, recvbuf:", rank);
    for (int i = 0; i < size; ++i)
    {
        printf("%d ", recvbuf[i]);
    }
    printf("\n");
    MPI_Finalize();
    return 0;
}