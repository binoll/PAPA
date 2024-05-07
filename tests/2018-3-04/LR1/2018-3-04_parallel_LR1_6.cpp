#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "mpi.h"


int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    
    int rank, procs, namelen;
    char proc_name[MPI_MAX_PROCESSOR_NAME];
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);

    MPI_Request request[procs - 1];
    MPI_Status status[procs - 1];

	int recvbuf[procs];
	int sendbuf[procs];
	int i = 0;

    while (i < procs)
    {
        sendbuf[i] = (i + 12) * rank + 1;
        i++;
    }

    int p_send = 0;
    int ids = 0;

    while (p_send < procs)
    {
        if (p_send == rank)
        {
            p_send++;
            continue;
        }

        MPI_Isend(&sendbuf[p_send], 1, MPI_INT, p_send, 1, MPI_COMM_WORLD, &request[ids]);
        ids++;
        p_send++;
    }

    int p_recv = 0;
    int idr = 0;

    while (p_recv < procs)
    {
        if (p_recv == rank)
        {
            recvbuf[p_recv] = sendbuf[p_recv];
            p_recv++;
            continue;
        }

        MPI_Recv(&recvbuf[p_recv], 1, MPI_INT, p_recv, 1, MPI_COMM_WORLD, &status[idr]);
        idr++;
        p_recv++;
    }

    MPI_Waitall(procs - 1, request, status);

    int k = 0;

    while (k < procs)
    {
        printf("%d ", sendbuf[k]);
        k++;
    }
    printf("|, ");

    int q = 0;

    while (q < procs)
    {
        printf("%d ", recvbuf[q]);
        q++;
    }
    printf("|\n");


	MPI_Finalize();
	return 0;
    
}
