#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"

#define MAX_BUF_SIZE 1024

void SenBar()
{
    char sendbuf[] = { '!' };
    char* recvbuf = new char(sizeof(sendbuf));

    int allnum;
    MPI_Comm_size(MPI_COMM_WORLD, &allnum);

    MPI_Request request[allnum];
    MPI_Status status[allnum];
   
    for (int i = 0; i < allnum; ++i)
    {
        MPI_Isend(sendbuf, sizeof(sendbuf), MPI_CHAR, i, 0, MPI_COMM_WORLD, &request[i]);
        MPI_Irecv(recvbuf, sizeof(recvbuf), MPI_CHAR, i, 0, MPI_COMM_WORLD, &request[i]);
    }

    MPI_Waitall(allnum, request, status);
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int rank, allnum;
    char proc_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &allnum);

    MPI_Request request;
    MPI_Status status;

    double starttime, endtime;

    sleep(rank);
	starttime = MPI_Wtime();
    SenBar();
	endtime = MPI_Wtime();
	if(rank == allnum - 1) printf("\nThat took %f seconds for SENfunction\n",endtime-starttime);
	
	printf("Message from %d proc - I'm awake!. Time is %f\n", rank, (endtime-starttime));
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	starttime = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
	endtime = MPI_Wtime();
	if(rank == 0) printf("\nThat took %f seconds for MPIfunction\n",endtime-starttime);

    MPI_Finalize();
}