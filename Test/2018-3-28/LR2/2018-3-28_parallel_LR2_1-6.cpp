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
    int rank, size, tag;
    double start, end, startStock, endStock;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Request* requests = new MPI_Request[size * 2];
    MPI_Status* status = new MPI_Status[size * 2];
    char* recvbuf = new char(sizeof(char) * size);
    char* recvbufStock = new char(sizeof(char) * size);
    char* message = new char(sizeof(char) * size);
    for (int i = 0; i < size; i++)
        message[i] = char(int('a') + i);
    cout << " Before rank: " << rank << " ";
    for (int i = 0; i < size; i++)
        cout << message[i] << " ";
    cout << endl;
    int iter = 0;
    start = MPI_Wtime();
    for (int i = 0; i < size; i++)
    {
        MPI_Isend(message + i, 1, MPI_CHAR, i, tag, MPI_COMM_WORLD, &requests[iter++]);
        MPI_Irecv(recvbuf + i, 1, MPI_CHAR, i, tag, MPI_COMM_WORLD, &requests[iter++]);
    }
    MPI_Waitall(iter, requests, status);
    end = MPI_Wtime();
    cout << "After Rank: " << rank << " ";
    for (int i = 0; i < size; i++)
        cout << recvbuf[i] << " ";
    cout << endl;
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0)
        cout << "Time work custom alltoall: " << end - start << " seconds" << endl;
    startStock = MPI_Wtime();
    MPI_Alltoall(message, 1, MPI_CHAR, recvbufStock, 1, MPI_CHAR, MPI_COMM_WORLD);
    endStock = MPI_Wtime();
    cout << "After stock Rank: " << rank << " ";
    for (int i = 0; i < size; i++)
        cout << recvbuf[i] << " ";
    cout << endl;
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0)
        cout << "Time work stock alltoall: " << endStock - startStock << " seconds" << endl;

    MPI_Finalize();
}
