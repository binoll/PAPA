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
    char* small_buffer;
    char* big_buffer;
    int size;
    while ((opt = getopt(argc, argv, "b:")) != -1) {
        switch (opt) {
        case 'b':
            big_buffer = malloc(strlen(optarg));
            small_buffer = malloc(strlen(optarg));
            strcpy(big_buffer, optarg);
            size = strlen(optarg);
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
    int i = 1;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc);
    if (myrank == 0) {
    	for(i; i<proc; i++){
            MPI_Send(big_buffer, size, MPI_CHAR, i, tag, MPI_COMM_WORLD);
            printf("Sending message: %s, rank: %d\n", big_buffer, myrank);
        }
    }
    else {
        MPI_Recv(small_buffer, size, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
        printf("Recieved message: %s, rank: %d\n", small_buffer, myrank);
     
    }
    MPI_Finalize();
}

