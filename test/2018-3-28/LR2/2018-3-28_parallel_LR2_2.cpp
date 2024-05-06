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
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int row[size];

    int send_row[size];
    int recv_col[size];

    for (int i = 0; i < size; ++i)
        send_row[i] = row[i] = rank;

    cout << "Rank: " << rank << ": ";
    for (int i = 0; i < size; i++)
        cout << send_row[i] << " ";
    cout << endl;
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "Rank: " << rank << " after multiply - ";
    for (int i = 0; i < size; ++i)
    {
        MPI_Allgather(&send_row[i], 1, MPI_INT, recv_col, 1, MPI_INT, MPI_COMM_WORLD);
        int sum = 0;
        for (int j = 0; j < size; ++j)
            sum += recv_col[j] * row[j];
        printf("%d ", sum);
    }
    cout << endl;

    MPI_Finalize();
}
