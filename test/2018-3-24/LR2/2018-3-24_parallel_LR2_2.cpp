#include "mpi.h"
#include <stdio.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int allnum, rank;
    if (MPI_Comm_rank(MPI_COMM_WORLD, &rank)) perror("Err at Comm_rank :( \n");
	if (MPI_Comm_size(MPI_COMM_WORLD, &allnum)) perror("Err at Comm_size :( \n");

    int size = 1;
    int row[allnum];

    int send_row[allnum];
    int recv_col[allnum];

	printf("\n Message in %d rank process to send: ", rank);
    for (int i = 0; i < allnum; ++i)
    {
        send_row[i] = row[i] = rank;
		printf("%d ", send_row[i]);
    }
    
	MPI_Barrier(MPI_COMM_WORLD);
	
    printf("\n Messages in %d rank process: recieved: ", rank);
    for (int i = 0; i < allnum; ++i)
    {
        MPI_Allgather(&send_row[i], size, MPI_INT, recv_col, size, MPI_INT, MPI_COMM_WORLD);
        int sum = 0;
        for (int j = 0; j < allnum; ++j)
            sum += recv_col[j] * row[j];
        printf("%d ", sum);
    }
    printf("\n");
    
    MPI_Finalize();
}