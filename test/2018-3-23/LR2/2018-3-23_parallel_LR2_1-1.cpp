/*
* MPI_Barrier
*/
#include "mpi.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[])
{    
    int opt, size;
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

    MPI_Init(&argc, &argv);

    int rank, n, i, it;


    double time_start, time_finish;
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Request req[2 * n];
    MPI_Status statuses[n];
    int ibuf[n];

    if (rank == 0)
    {
        for (i = 1; i < n; i++)
        {
            MPI_Recv_init(&ibuf[i], 1, MPI_INT, i, 5, MPI_COMM_WORLD, &req[i]);
            MPI_Send_init(&rank, 1, MPI_INT, i, 6, MPI_COMM_WORLD, &req[n + i]);
        }
        time_start = MPI_Wtime();
        for (it = 0; it < size; it++) {
            MPI_Startall(n - 1, &req[1]);
            MPI_Waitall(n - 1, &req[1], statuses);
            MPI_Startall(n - 1, &req[n + 1]);
            MPI_Waitall(n - 1, &req[n + 1], statuses);
        }
    }
    else 
    {
        MPI_Recv_init(&ibuf[0], 1, MPI_INT, 0, 6, MPI_COMM_WORLD, &req[0]);
        MPI_Send_init(&rank, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &req[1]);
        time_start = MPI_Wtime();
        for (it = 0; it < size; it++) 
        {
            MPI_Start(&req[1]);
            MPI_Wait(&req[1], statuses);
            MPI_Start(&req[0]);
            MPI_Wait(&req[0], statuses);
        }
    }
    time_finish = MPI_Wtime() - time_start;
    printf("Rank = %d Time my function: = %lf\n", rank, time_finish / size);
    time_start = MPI_Wtime();
    for (it = 0; it < size; it++) MPI_Barrier(MPI_COMM_WORLD);
    time_finish = MPI_Wtime() - time_start;
    printf("rank = %d MPI_Barrier time = %lf\n", rank, time_finish / size);
    MPI_Finalize();
}