#include <stdio.h>
#include "mpi.h"
#include <unistd.h>


int main(int argc, char* argv[])
{
    int opt, n, tag, rank, bufsize, i;
    double starttime, endtime;
    while ((opt = getopt(argc, argv, "s:")) != -1)
    {
        switch (opt)
        {
        case 's':
            bufsize = atoi(optarg);
            break;
        }
    }

    int* message; message = (int*)malloc(bufsize * sizeof(int));
    int* recvbuf; recvbuf = (int*)malloc(bufsize * sizeof(int));
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (i = 0; i < bufsize; i++)
        message[i] = i * rank;

    printf("From rank %d: ", rank);
    for (i = 0; i < bufsize; i++)
        printf("%d ", message[i]);
    printf("\n");

    MPI_Reduce(message, recvbuf, bufsize, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("\nResult from rank 0: ");
        for (i = 0; i < bufsize; i++)
            printf("%d ", recvbuf[i]);
        printf("\n");
    }

    MPI_Finalize();
}