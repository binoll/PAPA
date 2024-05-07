%% writefile task2_1.c
/* spmd.c
 * ... illustrates the single program multiple data
 *      (SPMD) pattern using basic MPI commands.
 *
 * Joel Adams, Calvin College, November 2009.
 *
 * Usage: mpirun -np 4 ./spmd
 *
 * Exercise:
 * - Compile and run.
 * - Compare source code to output.
 * - Rerun, using varying numbers of processes
 *    (i.e., vary the argument to 'mpirun -np').
 * - Explain what "multiple data" values this
 *    "single program" is generating.
 */

#include <stdio.h>
#include <unistd.h>// printf()
#include <mpi.h>     // MPI functions

    int main(int argc, char** argv) {
    int size, rank;
    double start, end, startStock, endStock;
    char message[] = { 'a' };
    char* recvbuf = new char(sizeof(message));
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Request request[size];
    MPI_Status status[size];

    start = MPI_Wtime();
    printf("rank: %d\n", rank);

    if (rank == 0)
        sleep(3);
    if (rank == 1)
        sleep(2);

    for (int i = 0, j = 0; i < size; i++)
    {
        MPI_Isend(message, sizeof(message), MPI_CHAR, i, 0, MPI_COMM_WORLD, &request[j++]);
        MPI_Irecv(recvbuf, sizeof(recvbuf), MPI_CHAR, i, 0, MPI_COMM_WORLD, &request[j++]);
    }
    MPI_Waitall(size, request, status);

    end = MPI_Wtime();
    printf("rank: %d, time: %f seconds\n", rank, end - start);

    startStock = MPI_Wtime();

    if (rank == 0)
        sleep(3);
    if (rank == 1)
        sleep(2);

    MPI_Barrier(MPI_COMM_WORLD);
    endStock = MPI_Wtime();
    printf("Proc %d passed barrier for %f seconds\n", rank, endStock - startStock);


    MPI_Finalize();
    return 0;
}
