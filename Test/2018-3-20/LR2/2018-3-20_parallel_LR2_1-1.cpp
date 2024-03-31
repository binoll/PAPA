#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

void My_Barrier()
{
    char sendbuf[] = { '1' };
    char* recvbuf = new char(sizeof(sendbuf));

    int procs;
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    MPI_Request request[procs];
    MPI_Status status[procs];
   
    for (int i = 0, j = 0; i < procs; i++)
    {
        MPI_Isend(sendbuf, sizeof(sendbuf), MPI_CHAR, i, 0, MPI_COMM_WORLD, &request[j++]);
        MPI_Irecv(recvbuf, sizeof(recvbuf), MPI_CHAR, i, 0, MPI_COMM_WORLD, &request[j++]);
    }

    MPI_Waitall(procs, request, status);
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    
    int rank, procs;
    char proc_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    MPI_Request request;
    MPI_Status status;

    double start, end;

    start = MPI_Wtime();

    if (rank == 1)
    {
        sleep(2);
    }

    My_Barrier();
    //MPI_Barrier(MPI_COMM_WORLD);


    end = MPI_Wtime();

    printf("My barrier rank -> %d, time -> %f\n", rank, (end - start));


    start = MPI_Wtime();
    if (rank == 1)
    {
        sleep(2);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    end = MPI_Wtime();

    printf("MPI_Barrier rank -> %d, time -> %f\n", rank, (end - start));


    

    MPI_Finalize();
    
}