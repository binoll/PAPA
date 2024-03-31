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
    double start, end, startStock, endStock;
    int rank, size;
    char* recvbuf = new char(sizeof(char));
    char* recvbufStock = new char(sizeof(char));
    MPI_Init(&argc, &argv);
    char* message = new char(size * sizeof(char));
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Status status;
    MPI_Request request;

    for (int i = 0; i < size; i++)
        message[i] = char(int('a') + i);

    start = MPI_Wtime();
    MPI_Irecv(recvbuf, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request);

    if (rank == 0)
    {
        for (int i = 0; i < size; i++)
        {
            MPI_Isend(&message[i], 1, MPI_CHAR, i, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
        }
    }


    MPI_Wait(&request, &status);
    end = MPI_Wtime();
    cout << "Rank " << rank << " recvbuf: " << recvbuf[0] << endl;
    if (rank == 0)
        cout << "Time work custom scatter: " << end - start << " seconds" << endl;

    startStock = MPI_Wtime();
    MPI_Scatter(message, 1, MPI_CHAR, recvbufStock, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
    endStock = MPI_Wtime();
    cout << "From rank: " << rank << ", Recbduf: " << recvbufStock[0] << endl;
    if (rank == 0)
        cout << "Time work stock scatter: " << endStock - startStock << " seconds" << endl;
    MPI_Finalize();
}