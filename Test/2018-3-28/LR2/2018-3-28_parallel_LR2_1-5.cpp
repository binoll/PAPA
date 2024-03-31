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
#include <mpi.h>  
#include <iostream>   // MPI functions

    using namespace std;

int main(int argc, char** argv) {
    int rank, size;
    double start, end, startStock, endStock;
    MPI_Init(&argc, &argv);
    MPI_Status status;
    MPI_Request request;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    char* message = new char(size * sizeof(char));
    char* recvbuf = new char(size * sizeof(char));
    char* recvbufStock = new char(size * sizeof(char));
    for (int i = 0; i < size; i++)
        message[i] = char(int('a') + i);

    start = MPI_Wtime();
    for (int i = 0; i < size; i++)
        MPI_Isend(message, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);

    for (int i = 0; i < size; i++)
        MPI_Irecv(recvbuf, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
    MPI_Wait(&request, &status);

    end = MPI_Wtime();

    cout << "Rank: " << rank << " ";
    for (int i = 0; i < size; i++)
        cout << recvbuf[i] << " ";
    cout << endl;
    if (rank == 0)
        cout << "Time work custom allgather: " << end - start << " seconds" << endl;

    startStock = MPI_Wtime();

    MPI_Allgather(message, 1, MPI_CHAR, recvbufStock, 1, MPI_CHAR, MPI_COMM_WORLD);

    endStock = MPI_Wtime();

    cout << "Rank: " << rank << " ";
    for (int i = 0; i < size; i++)
        cout << recvbuf[i] << " ";
    cout << endl;
    if (rank == 0)
        cout << "Time work stock allgather: " << endStock - startStock << " seconds" << endl;


    MPI_Finalize();
}