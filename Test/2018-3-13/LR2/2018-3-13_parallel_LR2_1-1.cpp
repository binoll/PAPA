#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include "mpi.h"
#define SIZE_BUF 1024

int Barrier() {
    int rank, procs, tag;
    char sendbuf[] = { "HELLO" };
    char* recvbuf = new char(sizeof(sendbuf));

    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Request requests[procs];
    MPI_Status status[procs];

    
    for (int i = 0, req_iter = 0; i < procs; i++) {
        MPI_Isend(sendbuf, strlen(sendbuf), MPI_CHAR, i, tag, MPI_COMM_WORLD, &requests[req_iter++]);
        MPI_Irecv(recvbuf, sizeof(recvbuf), MPI_CHAR, i, tag, MPI_COMM_WORLD, &requests[req_iter++]);
    }
    MPI_Waitall(procs, requests, status);
}


int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int rank, procs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    double start, end;

    int time_sleep = 3;
    start = MPI_Wtime();
    if (rank == 0) {
        sleep(time_sleep);
    }

    printf("after barrier rank %d\n", rank);

    //MPI_Barrier();
    Barrier();

    end = MPI_Wtime();

    printf("before barrier rank %d\n time:%f\n", rank, (end-start)*1000);
    

    MPI_Finalize();
}
