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
    char* message = new char(sizeof(char));
    message = "a";

    MPI_Init(&argc, &argv);
    MPI_Status status;
    MPI_Request request;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    char* recvbuf = new char(size * sizeof(char));
    char* recvbufStock = new char(size * sizeof(char));

    start = MPI_Wtime();
    MPI_Isend(message, sizeof(message), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request);

    if (rank == 0)
    {
        for (int i = 0; i < size; i++)
        {
            MPI_Irecv(&recvbuf[i], sizeof(recvbuf), MPI_CHAR, i, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);

            cout << "Message from rank: " << i << ". Recv buffer: " << recvbuf[i] << endl;
        }
    }

    MPI_Wait(&request, &status);
    end = MPI_Wtime();
    if (rank == 0)
        cout << "Time work custom gather: " << end - start << " seconds" << endl;

    startStock = MPI_Wtime();

    MPI_Gather(message, 1, MPI_CHAR, recvbufStock, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        cout << endl << "Recv buffer: ";
        for (int i = 0; i < size; i++)
            cout << recvbufStock[i] << " ";
        cout << endl;


    }

    endStock = MPI_Wtime();
    if (rank == 0)
        cout << "Time work custom gather: " << endStock - startStock << " seconds" << endl;
    MPI_Finalize();
}
