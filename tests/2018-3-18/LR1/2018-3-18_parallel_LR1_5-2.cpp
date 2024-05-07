#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define MSG_SIZE 512

int main(int argc, char* argv[])
{
    int rank, size, tag = 777;
    int len = (int)strlen(argv[1]);
    char *rcv = (char*)malloc(len * sizeof(char));;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status stat;

    if(rank == 0){
        for(int src = 1; src < size; src++){
            MPI_Recv(rcv, len, MPI_CHAR, src, tag, MPI_COMM_WORLD, &stat);
            printf("From rank %i: %s\n", src, rcv);
        }
        free(rcv);
    }
    else{
        MPI_Send(argv[1], len, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}
