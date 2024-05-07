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
    char* recvbuf = new char(sizeof(char));
    char* recvbufStock = new char(sizeof(char));
    message = "1";

    MPI_Init(&argc, &argv);
    MPI_Status status;
    MPI_Request request;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    start = MPI_Wtime();
    if (rank == 0)
    {
        cout << "Rank: " << rank << " send message: " << message << endl;
        for (int i = 0; i < size; i++)
            MPI_Isend(message, sizeof(message), MPI_CHAR, i, 0, MPI_COMM_WORLD, &request);
    }
    else
    {
        MPI_Irecv(recvbuf, sizeof(recvbuf), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        cout << "Rank: " << rank << " recv mesage " << recvbuf << endl;
    }
    end = MPI_Wtime();
    if (rank == 0)
        cout << "Time craft BCast: " << end - start << "seconds" << endl;

    startStock = MPI_Wtime();
    MPI_Bcast(&recvbufStock, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
    endStock = MPI_Wtime();
    cout << "Rank: " << rank << " got message:" << recvbufStock[0] << "from rank 0" << endl;
    cout << "Time work BCastStock:" << endStock - startStock << " seconds" << endl;
    MPI_Finalize();
    return 0;
}
