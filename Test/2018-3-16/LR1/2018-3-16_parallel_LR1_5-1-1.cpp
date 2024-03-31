#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
    int opt;
    int myrank;
    char* recvbuf;
    char* big_buffer;
    int size;
    while ((opt = getopt(argc, argv, "b:")) != -1) {
        switch (opt) {
        case 'b':
            big_buffer = malloc(strlen(optarg));
            recvbuf = malloc(strlen(optarg));
            size = strlen(optarg);
            strcpy(big_buffer, optarg);

            break;

        default:
            fprintf(stderr, "Use flag -b to input message \n");
            exit(EXIT_FAILURE);
        }



    }
    MPI_Init(&argc, &argv);
    MPI_Status status;
    int tag = 0;
    int proc;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc);
    if (myrank == 0)
    {
        MPI_Send(big_buffer, size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        printf("Send message in rank %d: %s\n", myrank, big_buffer);
    }
    else
    {
        if (myrank + 1 < proc)
        {
            MPI_Recv(recvbuf, size, MPI_CHAR, myrank - 1, 0, MPI_COMM_WORLD, &status);
            printf("Received message in rank %d: %s\n", myrank, recvbuf);
            MPI_Send(recvbuf, size, MPI_CHAR, myrank + 1, 0, MPI_COMM_WORLD);
            printf("Send message in rank %d: %s\n", myrank + 1, recvbuf);
        }
        else
        {
            MPI_Recv(recvbuf, size, MPI_CHAR, myrank - 1, 0, MPI_COMM_WORLD, &status);
            printf("Received message in rank %d: %s\n", myrank, recvbuf);
            MPI_Send(recvbuf, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            printf("Send message in rank %d: %s\n", 0, recvbuf);
        }
    }
 
 

    MPI_Finalize();
}