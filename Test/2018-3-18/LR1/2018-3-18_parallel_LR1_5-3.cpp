#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define MSG_SIZE 512

int main(int argc, char* argv[])
{
    int rank, size, tag = 777;
    int len = (int)strlen(argv[1]);
    char *rcv = (char*)malloc(len * sizeof(char));

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status stat;
    MPI_Request req;

    for(int n = 0; n < size; n++){
        if(n == rank){
            continue;
        }
        MPI_Isend(argv[1], len, MPI_CHAR, n, tag, MPI_COMM_WORLD, &req);
        MPI_Irecv(rcv, len, MPI_CHAR, n, tag, MPI_COMM_WORLD, &req);
        MPI_Wait(&req, &stat);
        printf("From rank %i in rank %i: %s\n", n, rank, rcv);
    }
    free(rcv);

    MPI_Finalize();
}


